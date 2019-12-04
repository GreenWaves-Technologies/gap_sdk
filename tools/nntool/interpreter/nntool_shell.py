# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import argparse
import configparser
import logging
import os
import pickle
from functools import partial

import numpy as np
from cmd2 import (Cmd, Cmd2ArgumentParser, CompletionItem, EmptyStatement,
                  with_argparser)
from cmd2.utils import cast as cmd2_cast

from execution.execute_graph import ExecutionProgress, execute
from execution.quantization_mode import QuantizationMode
from generation.code_generator import (DEFAULT_GEN_OPTS,
                                       DEFAULT_GEN_OPTS_DESCRIPTIONS,
                                       CodeGenerator)
from generation.naming_convension import DefaultNamingConvension
from generation.default_template import default_template, dynamic_template
from graph.matches.matches import get_std_match_group, get_fusion, get_fusions
from importer.importer import create_graph
from quantization.cross_layer_range_eq import weight_equalization
from quantization.simple_auto_quantify import SimpleQuantizer
from quantization.tuneq import tuneq
from reports.activation_reporter import ActivationReporter
from reports.error_reporter import ErrorReporter
from reports.filter_reporter import (FilterDetailedStatsReporter,
                                     FilterStatsReporter)
from reports.quantization_reporter import QuantizationReporter
from reports.temps_reporter import TempsReporter
from reports.graph_reporter import GraphReporter
from stats.activation_stats_collector import ActivationStatsCollector
from stats.error_stats_collector import ErrorStatsCollector
from stats.filter_stats_collector import (FilterDetailedStatsCollector,
                                          FilterStatsCollector)
from stats.temps_stats_collector import TempsStatsCollector
from utils.data_importer import MODES, import_data
from utils.gap_tensor_file import read_gap_tensors
from utils.intermediate_cache import IntermediateCache
from utils.new_param_state import STATE_EXTENSION, dump_state, load_state
from utils.stats_funcs import STATS_BITS, qsnr

from .shell_utils import (NNToolShellLogHandler, filter_dirs, find_choice,
                          format_dump_file, glob_input_files, input_options,
                          output_table, print_comparison, table_options)

LOG = logging.getLogger("nntool")

CHECK_GRAPH_ERROR = """
A graph must be opened to use this command. Use the open command to open a graph.
"""

CHECK_QUANTIZED_ERROR = """
The opened graph must be quantized to use this command. Run the aquant command.
"""

CHECK_ADJUSTED_ERROR = """
The opened graph must be adjusted to use this command. Run the adjust command.
"""

VALID_LOG_LEVELS = [
    "INFO",
    "DEBUG",
    "WARNING"
]

EXTRA_PROPERTIES = {
    'log_level': 'set logging level (one of {} or number)'.format(", ".join(VALID_LOG_LEVELS)),
    'enable_cache' : 'enable value caching',
    'dequantize' : 'dequantize TFLITE grath on load',
    'fusions' : 'run standard graph fusions on graph load',
    'adjust_order' : 'adjust activation and parameter dimension order\
         to match autotiler on graph load',
    'weight_equalization' : 'equalize weights on graph load',
    'equalization_threshold': 'threshold for weight equalization convergence',
    'adjust_image' : 'adjust image input size and channels',
    'image_width' : 'input image width',
    'image_height' : 'input image height',
    'image_mode' : 'input image mode (one of {})'.format(", ".join(MODES.keys())),
    'input_divisor': "divide input tensor values by this value",
    'input_offset': "add this value to input tensor values",
    'input_norm_func': "lambda function in the form x: fn(x) where x is any input",
    'graph_name': 'name of the graph used for code generation',
    'template_file': 'template file used for code generation'
}

NO_GRAPH = {
    'G': None,
    'graph_file': "",
    'tensor_file': ""
}

# pylint: disable=too-many-public-methods
class NNToolShell(Cmd):
    intro = 'Welcome to NNTOOL. Type help or ? to list commands.\n'
    prompt = '(NNT) '

    def __init__(self, args, nntool_workdir, *rest, **kwargs):
        super().__init__(*rest, **kwargs)
        self._nntool_workdir = nntool_workdir
        self.settable.update(EXTRA_PROPERTIES)
        self.settable.update(DEFAULT_GEN_OPTS_DESCRIPTIONS)

        self.settings = {
            'enable_cache': True,
            'cache_dir': './.value_cache',
            'dequantize': False,
            'fusions': False,
            'adjust_order': False,
            'weight_equalization': False,
            'equalization_threshold': 0.1,
            'adjust_image': False,
            'image_width': -1,
            'image_height': -1,
            'image_mode': "",
            'image_transpose': False,
            'input_norm_func': "",
            'input_divisor': 128,
            'input_offset': -1,
            'input_shift': 0,
            'log_level': 'INFO',
            'graph_file': "",
            'tensor_file': "",
            'template_file': ""
        }

        self.settings.update(DEFAULT_GEN_OPTS)

        self.execute_load_settings()

        if args and args.log_level is not None:
            self.settings['log_level'] = args.log_level.upper()

        self._graph_idx = 0
        self._tensor_store = {}
        self.py_locals['tensors'] = self._tensor_store

        # settings overide graph file
        graph_file = self.settings['graph_file']
        tensor_file = self.settings['tensor_file']

        # command line overides that
        if args:
            if args.graph_file:
                graph_file = args.graph_file

            if args.tensor_file:
                tensor_file = args.tensor_file

            if args.template_file:
                self.settings['template_file'] = args.template_file

        if 'log_level' not in self.settings:
            self.settings['log_level'] = "INFO"

        if graph_file:
            self._graphs = []
            self._startup_commands.append(self.__build_open_graph(
                graph_file,
                tensor_file,
                self.dequantize
            ))
        else:
            self._graphs = [
                NO_GRAPH.copy()
            ]

        ExecutionProgress().listen(partial(NNToolShell.progress, self))
        LOG.propagate = False
        handler = NNToolShellLogHandler(self)
        formatter = logging.Formatter('%(module)s - %(message)s')
        handler.setFormatter(formatter)
        LOG.addHandler(handler)
        LOG.setLevel(self.settings['log_level'])

    @property
    def tensor_store_names(self):
        return self._tensor_store.keys()

    def progress(self, step_idx, name, is_cached=False):
        del is_cached
        if not step_idx:
            return
        print("{}\r{} {}\r".format(" " * 70, step_idx, name), end="")

    def __getattr__(self, k):
        if k in DEFAULT_GEN_OPTS:
            return self.settings[k]

    def __setattr__(self, k, val):
        if k in DEFAULT_GEN_OPTS:
            self.settings[k] = val
        super(NNToolShell, self).__setattr__(k, val)

    def run_script(self, script_path):
        expanded_path = os.path.abspath(os.path.expanduser(script_path))

        # Make sure the path exists and we can access it
        if not os.path.exists(expanded_path):
            self.perror("'{}' does not exist or cannot be accessed".format(expanded_path))
            return

        # Make sure expanded_path points to a file
        if not os.path.isfile(expanded_path):
            self.perror("'{}' is not a file".format(expanded_path))
            return

        # An empty file is not an error, so just return
        if os.path.getsize(expanded_path) == 0:
            return

        try:
            # Read all lines of the script
            with open(expanded_path, encoding='utf-8') as target:
                script_commands = target.read().splitlines()
        except OSError as ex:  # pragma: no cover
            self.pexcept("Problem accessing script from '{}': {}".format(expanded_path, ex))
            return

        orig_script_dir_count = len(self._script_dir)

        try:
            self.runcmds_plus_hooks(self._startup_commands)
            self._startup_commands.clear()
            self._script_dir.append(os.path.dirname(expanded_path))
            return self.runcmds_plus_hooks(script_commands)

        finally:
            with self.sigint_protection:
                # Check if a script dir was added before an exception occurred
                if orig_script_dir_count != len(self._script_dir):
                    self._script_dir.pop()


    # HELPERS / Properties

    @property
    def G(self):
        return self._graphs[self._graph_idx]['G']

    @G.setter
    def G(self, val):
        self._graphs[self._graph_idx]['G'] = val

    @property
    def graph_name(self):
        if self._graph_idx is None:
            return ""
        return self._graphs[self._graph_idx]['G'].name

    @graph_name.setter
    def graph_name(self, val):
        if self._graph_idx is None:
            return
        self._graphs[self._graph_idx]['G'].name = val

    @property
    def is_quantized(self):
        return self._graphs[self._graph_idx]['G'].quantization is not None

    @property
    def is_adjusted(self):
        return self._graphs[self._graph_idx]['G'].graph_identity.is_adjusted

    @property
    def is_equalized(self):
        return self._graphs[self._graph_idx]['G'].graph_identity.is_equalized

    @property
    def graph_file(self):
        return self._graphs[self._graph_idx]['graph_file']

    @graph_file.setter
    def graph_file(self, val):
        self._graphs[self._graph_idx]['graph_file'] = val

    @property
    def tensor_file(self):
        return self._graphs[self._graph_idx]['tensor_file']

    @tensor_file.setter
    def tensor_file(self, val):
        self._graphs[self._graph_idx]['tensor_file'] = val

    def other_open_graphs(self, only_open=False):
        items = []
        for graph_idx, graph in enumerate(self._graphs):
            if graph_idx == self._graph_idx:
                continue
            if graph['G'] is None:
                if only_open:
                    continue
                name = "No Graph"
            else:
                name = graph['G'].name
            items.append(CompletionItem(graph_idx, name))
        return items

    def _check_graph(self):
        if self.G is None:
            self.perror(CHECK_GRAPH_ERROR)
            raise EmptyStatement()

    def _check_quantized(self):
        if not self.is_quantized:
            self.perror(CHECK_QUANTIZED_ERROR)
            raise EmptyStatement()

    def _update_prompt(self):
        self.prompt = "(NNT {} {}) ".format(os.path.basename(self.graph_file),
                                            self._graph_idx)

    def _check_adjusted(self):
        if not self.is_adjusted:
            self.perror(CHECK_ADJUSTED_ERROR)
            raise EmptyStatement()

    def _get_input_args(self, args):
        res = {}
        if self.settings['adjust_image']:
            res['width'] = self.settings['image_width']
            res['height'] = self.settings['image_height']
            res['mode'] = self.settings['image_mode']
        else:
            res['width'] = res['height'] = -1
            res['mode'] = None
        if args:
            if args.width is not None:
                res['width'] = args.width
            if args.height is not None:
                res['height'] = args.height
            if args.mode is not None:
                res['mode'] = args.mode

#           res['shift'] = self.settings['input_shift'] if args.bit_shift
#           is None else args.bit_shift
            res['divisor'] = self.settings['input_divisor']\
                if args.divisor is None else args.divisor
            res['offset'] = self.settings['input_offset']\
                if args.offset is None else args.offset
            res['transpose'] = self.settings['image_transpose'] if args.transpose is None\
                else args.transpose
            res['norm_func'] = self.settings['input_norm_func'] if args.norm_func is None\
                else args.norm_func
        else:
#            res['shift'] = self.settings['input_shift']
            res['divisor'] = self.settings['input_divisor']
            res['offset'] = self.settings['input_offset']
            res['transpose'] = self.settings['image_transpose']
            res['norm_func'] = self.settings['input_norm_func']

        return res

    # STATUS COMMAND
    def do_status(self, _):
        """
Show current graph status
"""
        if self.G is None:
            self.poutput("No graph loaded")
            return
        self.poutput("graph file: {}".format(self.graph_file))
        if self.tensor_file:
            self.poutput("tensor file: {}".format(self.tensor_file))
        self.poutput("is_quantized: {}".format(self.is_quantized))
        for k in ['is_adjusted', 'is_equalized', 'is_fused']:
            self.poutput("{}: {}".format(k, getattr(self.G.graph_identity, k)))

    # SAVING AND LOADING SETTINGS
    def execute_save_settings(self, dirname=None):
        config = configparser.ConfigParser()
        config['NNTOOL'] = {k: str(getattr(self, k))\
            for k in self.settable if k != "prompt"}
        config['GRAPH'] = {k: getattr(self, k) for k in ['graph_file', 'tensor_file']}
        if dirname is None:
            dirname = self._nntool_workdir
        with open(os.path.join(dirname, 'nntool.ini'), 'w') as config_fp:
            config.write(config_fp)

    def execute_load_settings(self, filepath=None):
        nntool_home = os.path.join(self._nntool_workdir, 'nntool.ini')
        if filepath is None:
            if os.path.isfile('nntool.ini'):
                filepath = 'nntool.ini'
            elif os.path.isfile(nntool_home):
                filepath = nntool_home
            else:
                return
        config = configparser.ConfigParser()
        config.read(filepath)
        for key in config['NNTOOL']:
            if key == "prompt":
                continue
            elif key in self.settings:
                orig_val = self.settings[key]
                self.settings[key] = cmd2_cast(orig_val, config['NNTOOL'][key])
            else:
                orig_val = getattr(self, key)
                setattr(self, key, cmd2_cast(orig_val, config['NNTOOL'][key]))

        self.settings["graph_file"] = config['GRAPH']['graph_file']
        self.settings["tensor_file"] = config['GRAPH']['tensor_file']

    # CACHE PROPERTIES
    def update_cache(self):
        if self.G is None:
            return
        if self.settings['enable_cache']:
            self.G.value_cache = IntermediateCache(self.settings['cache_dir'])
        else:
            self.G.value_cache = None

    @property
    def enable_cache(self):
        return self.settings['enable_cache']

    @enable_cache.setter
    def enable_cache(self, val):
        self.settings['enable_cache'] = val
        self.update_cache()

    # CACHE_DIR COMMAND
    cache_dir_parser = Cmd2ArgumentParser(description="set value cache directory")
    cache_dir_parser.add_argument('value', nargs=argparse.OPTIONAL,
                                  help='the directory for the cache dir',
                                  completer_method=partial(Cmd.path_complete,
                                                           path_filter=filter_dirs))

    @property
    def cache_dir(self):
        return self.settings['cache_dir']

    @cache_dir.setter
    def cache_dir(self, val):
        if self.settings['cache_dir'] != val:
            self.settings['cache_dir'] = val
            self.update_cache()

    @with_argparser(cache_dir_parser)
    def do_cache_dir(self, args: argparse.Namespace):
        """
Sets the directory for the value cache
"""
        self.cache_dir = args.value

    # SAVE_SETTINGS COMMAND
    save_settings_parser = Cmd2ArgumentParser(description="set value cache directory")
    save_settings_parser.add_argument('value', nargs=argparse.OPTIONAL,
                                      help='the directory for the settings file',
                                      completer_method=partial(Cmd.path_complete,
                                                               path_filter=filter_dirs))

    @with_argparser(save_settings_parser)
    def do_save_settings(self, args: argparse.Namespace):
        """
Save the current settings. If no directory is given then they are saved
in the ~/.nntool directory"""
        self.execute_save_settings(args.value)

    # LOG_LEVEL PROPERTY

    @property
    def log_level(self):
        return self.settings['log_level']

    @log_level.setter
    def log_level(self, val):
        try:
            val = int(val)
            self.settings['log_level'] = val
        except ValueError:
            val = val.upper()
            val = find_choice(VALID_LOG_LEVELS, val)

        self.settings['log_level'] = val
        LOG.setLevel(self.settings['log_level'])
        LOG.info("set log level to %s", val)

    # DEQUANTIZE PROPERTY

    @property
    def dequantize(self):
        return self.settings['dequantize']

    @dequantize.setter
    def dequantize(self, val):
        self.settings['dequantize'] = bool(val)

    # FUSIONS PROPERTY

    @property
    def fusions(self):
        return self.settings['fusions']

    @fusions.setter
    def fusions(self, val):
        self.settings['fusions'] = bool(val)

    # ADJUST_ORDER PROPERTY

    @property
    def adjust_order(self):
        return self.settings['adjust_order']

    @adjust_order.setter
    def adjust_order(self, val):
        self.settings['adjust_order'] = bool(val)

    # WEIGHT_EQUALIZATION PROPERTY

    @property
    def weight_equalization(self):
        return self.settings['weight_equalization']

    @weight_equalization.setter
    def weight_equalization(self, val):
        self.settings['weight_equalization'] = bool(val)

    # EQUALIZATION_THRESHOLD PROPERTY

    @property
    def equalization_threshold(self):
        return self.settings['equalization_threshold']

    @equalization_threshold.setter
    def equalization_threshold(self, val):
        self.settings['equalization_threshold'] = float(val)

    # IMAGE ADJUSTMENT SETTINGS

    # ADJUST_IMAGE PROPERTY

    @property
    def adjust_image(self):
        return self.settings['adjust_image']

    @adjust_image.setter
    def adjust_image(self, val):
        self.settings['adjust_image'] = bool(val)

    # IMAGE_WIDTH PROPERTY

    @property
    def image_width(self):
        return self.settings['image_width']

    @image_width.setter
    def image_width(self, val):
        try:
            val = int(val)
            if val <= 0:
                raise ValueError()
        except ValueError:
            raise ValueError("value should be positive integer")
        self.settings['image_width'] = bool(val)

    # IMAGE_HEIGHT PROPERTY

    @property
    def image_height(self):
        return self.settings['image_height']

    @image_height.setter
    def image_height(self, val):
        try:
            val = int(val)
            if val <= 0:
                raise ValueError()
        except ValueError:
            raise ValueError("value should be positive integer")
        self.settings['image_height'] = bool(val)

    # IMAGE_MODE PROPERTY

    @property
    def image_mode(self):
        return self.settings['image_mode']

    @image_mode.setter
    def image_mode(self, val):
        val = find_choice(MODES.keys(), val)
        self.settings['image_mode'] = bool(val)

    # INPUT_DIVISOR PROPERTY

    @property
    def input_divisor(self):
        return self.settings['input_divisor']

    @input_divisor.setter
    def input_divisor(self, val):
        self.settings['input_divisor'] = int(val)

    # INPUT_NORM_FUNC PROPERTY

    @property
    def input_norm_func(self):
        return self.settings['input_norm_func']

    @input_norm_func.setter
    def input_norm_func(self, val):
        self.settings['input_norm_func'] = str(val)

    # INPUT_OFFSET PROPERTY

    @property
    def input_offset(self):
        return self.settings['input_offset']

    @input_offset.setter
    def input_offset(self, val):
        self.settings['input_offset'] = int(val)

    @property
    def template_file(self):
        return self.settings['template_file']


    @template_file.setter
    def template_file(self, val):
        self.settings['template_file'] = val

    # OPEN COMMAND
    parser_open = Cmd2ArgumentParser("open a graph file")
    parser_open.add_argument('nnfile',
                             completer_method=Cmd.path_complete,
                             help='graph or state file',
                             metavar="INPUT_GRAPH or STATE_FILE")
    parser_open.add_argument('tensor_file',
                             nargs=argparse.OPTIONAL,
                             completer_method=Cmd.path_complete,
                             help='optional tensor file')
    parser_open.add_argument('-d', '--dequantize',
                             help='dequantize TFLite tensors', action='store_true')
    parser_open.add_argument('-t', '--tflite_version',
                             help='dequantize TFLite tensors',
                             choices=['v3', 'head'],
                             default='v3')
    parser_open.add_argument('-n', '--new',
                             help='open as new graph - keep existing graph open',
                             action='store_true')

    @staticmethod
    def __build_open_graph(graph_file, tensor_file, dequantize):
        command = ["open", graph_file, "-n"]
        if tensor_file:
            command.append("-t {}".format(tensor_file))
        if dequantize:
            command.append("-d")
        return " ".join(command)

    def __open_graph(self, graph_file, tensor_file, tflite_version, dequantize):

        value_cache = IntermediateCache(self.settings['cache_dir'])\
            if self.settings['enable_cache'] else None

        _, ext = os.path.splitext(graph_file)

        if ext == STATE_EXTENSION:
            LOG.info("opening state file %s", graph_file)
            self.graph_file = graph_file
            self.G, extra = load_state(graph_file, value_cache=value_cache, return_extra=True)
            self.settings.update(extra)
        else:
            LOG.info("opening graph file %s", graph_file)
            opts = {
                'load_tensors': True,
                'dequantize': dequantize,
                'value_cache': value_cache,
                'tflite_schema': tflite_version
            }

            G = create_graph(graph_file, opts=opts)
            G.add_dimensions()
            if tensor_file:
                G.load_tensors(tensor_file)
            self.G = G
            self.graph_file = graph_file
            if tensor_file is not None:
                self.tensor_file = tensor_file
            self.settings['dequantize'] = bool(dequantize)
            if self.settings['adjust_order']:
                LOG.info("adjusting order")
                self.execute_adjust_order()
            if self.settings['fusions']:
                LOG.info("applying standard fusions")
                self.apply_standard_fusions()
            if self.settings['weight_equalization']:
                LOG.info("equalizing weights")
                weight_equalization(self.G, self.settings['equalization_threshold'])

    @with_argparser(parser_open)
    def do_open(self, args: argparse.Namespace):
        """
Open a graph or state file"""
        if args.new:
            # reset the current graph
            self._graphs.append(NO_GRAPH.copy())
            self._graph_idx = len(self._graphs) - 1
        else:
            # reset the current graph
            self._graphs[self._graph_idx] = NO_GRAPH.copy()
        self.__open_graph(args.nnfile, args.tensor_file, args.tflite_version, args.dequantize)
        self._update_prompt()
        self.py_locals['G'] = self.G

    # GRAPH COMMAND
    parser_graph = Cmd2ArgumentParser("display graph")
    parser_graph.add_argument('graph_number',
                              nargs=argparse.OPTIONAL,
                              type=int,
                              choices_method=other_open_graphs,
                              help='graph to select or nothing to show open graphs')

    @with_argparser(parser_graph)
    def do_graph(self, args: argparse.Namespace):
        """
Select actuve graphs"""
        if args.graph_number is not None:
            if args.graph_number < 0 or args.graph_number >= len(self._graphs):
                self.perror("graph number is invalid")
                return
            self._graph_idx = args.graph_number
            self.pfeedback("selected graph {}".format(self._graph_idx))
            self._update_prompt()
            self.py_locals['G'] = self.G
        else:
            for idx, rec in enumerate(self._graphs):
                self.poutput("{:d} - {}".format(idx, rec['graph_file']))

    # SHOW COMMAND
    parser_show = Cmd2ArgumentParser("display graph")
    table_options(parser_show)

    @with_argparser(parser_show)
    def do_show(self, args: argparse.Namespace):
        """
Display the structure of the graph"""
        self._check_graph()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        split_dims = fmt == "xls"
        do_totals = fmt != "csv"

        tab = GraphReporter(split_dims=split_dims, do_totals=do_totals).report(self.G, None)
        output_table(tab, args)

    # STATS COMMAND
    parser_stats = Cmd2ArgumentParser("display statistics on globals")
    parser_stats.add_argument('-d', '--detailed',
                              action="store_true", help='Dump detailed statistics')
    parser_stats.add_argument('-q', '--qsnr',
                              type=float, default=30.0, help='QSNR threshold')
    table_options(parser_stats, default_width=180)

    @with_argparser(parser_stats)
    def do_stats(self, args: argparse.Namespace):
        """
Display statistics on weights and biases"""
        self._check_graph()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        if args.detailed:
            stats_collector = FilterDetailedStatsCollector()
            stats = stats_collector.collect_stats(self.G)
            tab = FilterDetailedStatsReporter().report(self.G, stats)
        else:
            stats_collector = FilterStatsCollector()
            stats = stats_collector.collect_stats(self.G)
            tab = FilterStatsReporter(do_totals=(fmt != "csv"), threshold=args.qsnr)\
                .report(self.G, stats)
        output_table(tab, args)


    # FUSIONS COMMAND
    def fusions_list(self):
        return [elem[0] for elem in get_fusions()]

    parser_fusions = Cmd2ArgumentParser("apply fusions to graph")
    parser_fustions_exclusive = parser_fusions.add_mutually_exclusive_group()
    parser_fustions_exclusive.add_argument('-l', '--list', action='store_true', help='list available fusions')
    parser_fustions_exclusive.add_argument('-a', '--apply', type=str, choices_method=fusions_list, help='apply a fusion')

    def apply_standard_fusions(self):
        get_std_match_group().match(self.G)
        self.G.add_dimensions()

    @with_argparser(parser_fusions)
    def do_fusions(self, args):
        """
Carry out the default set of fusions on the graph"""
        self._check_graph()
        if args.list:
            self.ppaged("\n".join(["%s - %s" % (name, desc) for name, desc in get_fusions()]))
            return
        if args.apply:
            fusion = get_fusion(args.apply)
            if not fusion:
                self.perror('fusion %s not found' % args.apply)
                return
        else:
            fusion = get_std_match_group()

        fusion.match(self.G)
        self.G.add_dimensions()
        self.G.quantization = None

    # ADJUST COMMAND
    # parser_adjust = Cmd2ArgumentParser("display statistics on globals")

    def execute_adjust_order(self):
        self.G.adjust_order()
        self.G.add_dimensions()

    # @with_argparser(parser_adjust)
    def do_adjust(self, _):
        """
Adjust activation and parameter tensors to match AutoTiler order.
Must be run before generating code."""
        self._check_graph()
        self.execute_adjust_order()

    # WEIGHT_EQUALIZATION COMMAND
    parser_we = Cmd2ArgumentParser()
    parser_we.add_argument('threshold',
                           type=float, default=0.1,
                           help='convergence threshold')

    def execute_weight_equalization(self, threshold):
        if not (threshold > 0 and threshold < 10):
            self.perror("threshold should be 10 > x > 0")
        weight_equalization(self.G, threshold=threshold)

    @with_argparser(parser_we)
    def do_weight_equalization(self, args: argparse.Namespace):
        """
Run weight equalization on graph. This reduces variance between weight
channels and may improve quantization accuracy."""
        self._check_graph()
        self.execute_weight_equalization(args.threshold)

    # ASTATS COMMAND
    parser_astats = Cmd2ArgumentParser()
    parser_astats.add_argument('-q', '--qsnr',\
        type=float, default=30.0, help='QSNR threshold')
    parser_astats.add_argument('-d', '--detail',\
        action="store_true", help='Show fusions detail')
    table_options(parser_astats, default_width=180)
    input_options(parser_astats)

    @with_argparser(parser_astats)
    def do_astats(self, args: argparse.Namespace):
        """
Calculate activation statistics on one or more imput files."""
        self._check_graph()
        input_args = self._get_input_args(args)
        stats_collector = ActivationStatsCollector()
        for input_file in glob_input_files(args.input_files):
            LOG.info("input file %s", input_file)
            data = import_data(input_file, **input_args)
            data = stats_collector.collect_stats(self.G, [data])

        fmt = ('tab' if args.output is None else args.output['fmt'])
        tab = ActivationReporter(do_totals=(fmt != "csv"),
                                 threshold=args.qsnr,
                                 yield_fusions=args.detail)\
            .report(self.G, stats_collector.reduce_stats())
        output_table(tab, args)

    # AQUANT COMMAND
    parser_aquant = Cmd2ArgumentParser()
    parser_aquant_group = parser_aquant.add_mutually_exclusive_group(required=True)
    parser_aquant_group.add_argument('-q', '--qsnr',\
        type=float, default=50.0, help='QSNR threshold')
    parser_aquant_group.add_argument('-f', '--force_width',\
        choices=STATS_BITS, type=int, help='force all layers to this width')
    table_options(parser_aquant, default_width=140)
    input_options(parser_aquant)

    @with_argparser(parser_aquant)
    def do_aquant(self, args: argparse.Namespace):
        """
Attempt to calculate quantization for graph using one or more sample imput files."""
        self._check_graph()
        input_args = self._get_input_args(args)
        processed_input = False
        stats_collector = ActivationStatsCollector()
        for input_file in glob_input_files(args.input_files):
            LOG.info("input file %s", input_file)
            processed_input = True
            data = import_data(input_file, **input_args)
            stats_collector.collect_stats(self.G, [data])
        if not processed_input:
            self.perror("No imput files found")
            return
        astats = stats_collector.reduce_stats()
        stats_collector = FilterStatsCollector()
        fstats = stats_collector.collect_stats(self.G)
        quantizer = SimpleQuantizer(astats, fstats,
                                    force_width=args.force_width,
                                    min_qsnr=args.qsnr)
        qrecs = quantizer.quantize(self.G)
        self.G.quantization = qrecs
        tab = QuantizationReporter().report(self.G, qrecs)
        output_table(tab, args)

    # DUMP COMMAND
    parser_dump = Cmd2ArgumentParser()
    parser_dump.add_argument('-s', '--step',
                             type=int, help='step to dump output of', default=None)
    parser_dump.add_argument('-w', '--number_width',
                             type=int, help='width of numbers', default=8)
    parser_dump.add_argument('-p', '--precision',
                             type=int, help='number of decimal places', default=4)
    parser_dump.add_argument('-c', '--channel',
                             type=int, help='channel to dump', default=None)
    parser_dump.add_argument('-d', '--dequantize',
                             action='store_true', help='dequantize result')
    parser_dump_group = parser_dump.add_mutually_exclusive_group(required=False)
    parser_dump_group.add_argument('-q', '--quantize', action='store_true',
                                   help='quantize the graph (must have already set quantization)')
    parser_dump_group.add_argument('-Q', '--quantize_step', type=int,
                                   help='quantize a step of the graph (must have already'+
                                   ' set quantization)',
                                   default=None)
    parser_dump.add_argument('-P', '--pickle',
                             completer_method=Cmd.path_complete,
                             help='pickle all the outputed tensors to this file')
    parser_dump.add_argument('-S', '--save',
                             help='save the tensor to the tensors list')
    input_options(parser_dump)

    @with_argparser(parser_dump)
    def do_dump(self, args: argparse.Namespace):
        """
Dump the activations resulting from running an input file through the graph.
You can use the current quantization settings and can also just quantify one
specific step of the graph."""
        self._check_graph()
        if args.quantize or args.quantize_step:
            self._check_quantized()
            if args.quantize:
                qmode = QuantizationMode.all()
            else:
                qmode = QuantizationMode.step(args.quantize_step)
        else:
            qmode = QuantizationMode.none()
        if args.step is not None:
            step = args.step
            num_steps = len(self.G.graph_state.steps)
            if step < 0:
                step = num_steps + step
            if step < 0 or step > num_steps:
                self.perror("step must be from {} to {}".format(-num_steps, num_steps))
                return
        else:
            step = None

        input_args = self._get_input_args(args)

        pickles = []
        dequantize = args.dequantize if args.dequantize is not None\
            else not (args.pickle or args.save)

        for input_file in glob_input_files(args.input_files):
            LOG.info("input file %s", input_file)

            data = import_data(input_file, **input_args)
            outputs = execute(self.G, [data], limit=step, qrecs=self.G.quantization,
                              qmode=qmode,
                              dequantize=dequantize)

            if args.pickle or self._in_py or args.save:
                pickles.append(format_dump_file(self.G, outputs))
            else:
                self.G.print_intermediates(outputs, limit=step, width=args.number_width,
                                           precision=args.precision, channel=args.channel,
                                           order=['c', 'h', 'w'])

        if args.pickle or args.save or self._in_py:
            if not pickles:
                self.perror("no input files found")
                return
            if len(args.input_files) == 1:
                pickles = pickles[0]
            if args.pickle:
                with open(args.pickle, 'wb') as pickle_fp:
                    pickle.dump(pickles, pickle_fp)
            if args.save:
                self._tensor_store[args.save] = pickles

        if self._in_py:
            self.last_result = pickles

    # TENSORS_COMMAND
    parser_tensors = Cmd2ArgumentParser()
    parser_tensors.add_argument('-c', '--channel',
                                nargs=(1, 2),
                                type=int,
                                help='channel to compare')
    parser_tensors.add_argument('-s', '--step',
                                type=int,
                                help='step to compare')
    parser_tensors.add_argument('-q', '--compare_qsnr',
                                action='store_true',
                                help='compare two tensors QSNR')
    parser_tensors.add_argument('-n', '--name',
                                type=str,
                                choices_method=lambda x: x.tensor_store_names,
                                help='name to use for the tensor in the tensor store')
    parser_texclu1 = parser_tensors.add_mutually_exclusive_group()
    parser_texclu1.add_argument('-W', '--weights',
                                action='store_true',
                                help='compare weights')
    parser_texclu1.add_argument('-B', '--biases',
                                action='store_true',
                                help='compare biases')
    parser_texclu2 = parser_tensors.add_mutually_exclusive_group()
    parser_texclu2.add_argument('-t', '--tensors',
                                nargs=2,
                                type=str,
                                choices_method=lambda x: x.tensor_store_names,
                                help='compare two tensors')
    parser_texclu2.add_argument('-g', '--gap_load',
                                completer_method=Cmd.path_complete,
                                help='load tensors dumped by autotiler code. '+
                                'Supply the filename and'+
                                ' an optional tensor store name. If none is given'+
                                ' the filename will be used.')
    parser_texclu2.add_argument('-X', '--clear',
                                action='store_true',
                                help='clears the tensor store')


    @with_argparser(parser_tensors)
    def do_tensors(self, args):
        """
Load and manipulate tensors. If no option is supplied the saved tensors will be listed.
All the tensors in the store are available in dictionary 'tensors' in the python console
accessed by the command 'py'."""
        if args.clear:
            self.pfeedback('tensor store cleared')
            self._tensor_store.clear()
            return
        if args.gap_load:
            store_name = args.gap_load if not args.name else args.name
            self._tensor_store[store_name] = read_gap_tensors(args.gap_load)
            return
        if args.tensors:
            compare = args.tensors
            tensors = [None]*2
            for i in range(2):
                tensors[i] = self._tensor_store.get(compare[i])
                if not tensors[i]:
                    self.perror("{} not in store".format(compare[i]))
                    return
                if args.weights:
                    tensors[i] = [t[1] for t in tensors[i]]
                elif args.biases:
                    tensors[i] = [t[2] for t in tensors[i]]
                else:
                    tensors[i] = [t[0] for t in tensors[i]]

            if args.step is not None:
                for i in range(2):
                    if args.step >= len(tensors[i]):
                        self.perror("{} doesn't have that step".format(compare[i]))
                        return
                    if tensors[i][args.step] is None:
                        self.perror("{} doesn't have this tensor for that step".format(compare[i]))
                        return
                    tensors[i] = [tensors[i][args.step]]

            if args.channel is not None:
                for i in range(2):
                    for j, tensor in enumerate(tensors[i]):
                        if len(tensor.shape) <= len(args.channel):
                            self.perror("selected too many channels for this tensor")
                        for c in args.channel:
                            tensor = tensor[c]
                        tensors[i][j] = tensor

            if args.compare_qsnr:
                out = [qsnr(t1.astype(np.float64), t2.astype(np.float64))\
                    for t1, t2 in zip(*tensors)]
                print(out)
            else:
                self.ppaged("\n".join(print_comparison(tensors)))

            return
        for idx, k in enumerate(self._tensor_store):
            print("{:3d}) {}".format(idx, k))

    # QSHOW COMMAND
    parser_qshow = Cmd2ArgumentParser()
    table_options(parser_qshow)

    @with_argparser(parser_qshow)
    def do_qshow(self, args):
        """
Show current quantization settings."""
        self._check_graph()
        self._check_quantized()
        tab = QuantizationReporter().report(self.G, self.G.quantization)
        output_table(tab, args)

    # GEN COMMAND
    parser_gen = Cmd2ArgumentParser()
    parser_gen.add_argument('model_file',
                            completer_method=Cmd.path_complete,
                            nargs=argparse.OPTIONAL,
                            help='file to write to, otherwise output to terminal')
    parser_gen.add_argument('-T', '--tensor_directory',
                            completer_method=Cmd.path_complete,
                            help='path to tensor directory. full path will be created'+\
                            ' if it doesn\'t exist. If this parameter is given it will'+\
                            'update the settings saved with the graph state.')
    parser_gen.add_argument('-M', '--model_directory',
                            completer_method=Cmd.path_complete,
                            help='path to model directory. full path will be created'+\
                            ' if it doesn\'t exist. If this parameter is given it will'+\
                            'update the settings saved with the graph state.')
    parser_gen.add_argument('-t', '--output_tensors',
                            action='store_true',
                            help='write constants (weights, biases)')
    parser_gen.add_argument('-c', '--checksums',
                            completer_method=Cmd.path_complete,
                            help='generate checksum tests in code for the given file')

    @with_argparser(parser_gen)
    def do_gen(self, args):
        """
Generate AutoTiler model C code and optionally dump tensors. If no destination file is
given the generated code will be outputed to the screen. Check the 'set' command for
settings related to code generation."""
        self._check_graph()
        self._check_quantized()
        self._check_adjusted()
        if args.checksums:
            input_args = self._get_input_args(None)
            LOG.info("input file %s", args.checksums)
            data = import_data(args.checksums, **input_args)
            execute(self.G, [data], qrecs=self.G.quantization, qmode=QuantizationMode.all())
            self.settings['checksum_file'] = args.checksums
            self.settings['generate_checksums'] = True

        if args.tensor_directory:
            self.settings['tensor_directory'] = args.tensor_directory
        if args.model_directory:
            self.settings['model_directory'] = args.model_directory
        code_gen = CodeGenerator(self.G, DefaultNamingConvension(self.G), self.settings)

        if self.settings['template_file']:
            code_template = dynamic_template(self.settings['template_file'])
        else:
            code_template = default_template

        if args.model_file:
            with open(os.path.join(self.settings['model_directory'],
                                   args.model_file), "w") as output_fp:
                output_fp.write(code_template(self.G, code_generator=code_gen))
        else:
            self.ppaged(code_template(self.G, code_generator=code_gen))
        if args.output_tensors:
            code_gen.write_constants()

    # SAVE_STATE COMMAND
    parser_save_state = Cmd2ArgumentParser()
    parser_save_state.add_argument('output',
                                   completer_method=Cmd.path_complete,
                                   nargs=argparse.OPTIONAL,
                                   help='file to write to')

    @with_argparser(parser_save_state)
    def do_save_state(self, args):
        """
Save the state of the transforms and quantization of the graph.
This state file can be used to generate the model file as part of
a build script. If no argument is given then the state files
will be saved in the same directory as the graph. If a directory is
given then the state files will be saved in it with the graph
basename. If a filename is given, its basename will be used to
save the state files."""
        self._check_graph()
        self._check_quantized()
        gen_opts = {k: self.settings[k] for k in DEFAULT_GEN_OPTS}
        dump_state(self.G, state_path=args.output, extra=gen_opts)

    # QERROR COMMAND
    parser_qerror = Cmd2ArgumentParser()
    parser_qerror.add_argument('-s', '--step',\
        type=int, help='step to evaluate', default=None)
    parser_qerror.add_argument('-r', '--report_lowest',\
        type=int, help='QSNR threshold below which to report filename')
    table_options(parser_qerror, default_width=140)
    input_options(parser_qerror)

    @with_argparser(parser_qerror)
    def do_qerror(self, args):
        """
Show quantization error introduced by processing one or more input files."""
        self._check_graph()
        self._check_quantized()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        input_args = self._get_input_args(args)
        stats_collector = ErrorStatsCollector()
        cnt = 0
        for filename in glob_input_files(args.input_files):
            cnt += 1
            data = import_data(filename, **input_args)
            stat = stats_collector.collect_stats(self.G, [data])
            if args.report_lowest is not None:
                lowest = min((elem['qsnr'] for elem in stat.values()))
                if lowest < args.report_lowest:
                    self.pfeedback("{} had QSNR below threshold".format(filename))

        tab = ErrorReporter(do_totals=(fmt != "csv"), one_input=cnt <= 1)\
            .report(self.G, stats_collector.reduce_stats())
        output_table(tab, args)

    # QTUNE COMMAND
    parser_tune = Cmd2ArgumentParser()
    parser_tune.add_argument('step',
                             type=int, help='step to tune')
    parser_tune.add_argument('parameter',
                             choices=['acc', 'calc', 'weights', 'dp'],
                             help='which parameter to tune')
    parser_tune.add_argument('X',
                             nargs='?',
                             default=0,
                             type=int, help='X of QX.Y')
    parser_tune.add_argument('Y',
                             nargs='?',
                             default=0,
                             type=int, help='Y of QX.Y')

    @with_argparser(parser_tune)
    def do_qtune(self, args):
        """
Tune quantization of graph."""
        self._check_graph()
        self._check_quantized()

        tuneq(self.G, self.G.quantization, args.step, args.parameter, args.X, args.Y)

    # TEMPS COMMAND
    parser_temps = Cmd2ArgumentParser()
    table_options(parser_temps, default_width=140)

    @with_argparser(parser_temps)
    def do_temps(self, args):
        """
Show statistics on activations."""
        self._check_graph()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        stats_collector = TempsStatsCollector()
        stats = stats_collector.collect_stats(self.G)
        tab = TempsReporter(do_totals=(fmt != "csv")).report(self.G, stats)
        output_table(tab, args)
