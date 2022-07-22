# Copyright (C) 2020, 2021  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import json
import argparse
from functools import reduce
from pathlib import Path

from cmd2 import Cmd, with_argparser
from nntool.interpreter.nntool_shell_base import (NODE_SELECTOR_HELP,
                                           NNToolArguementParser,
                                           NNToolShellBase)
from nntool.quantization.handlers_helpers import get_all_options_by_params
from nntool.quantization.quantizer.new_quantizer import NewQuantizer
from nntool.utils.argparse_kwargs import kwargs_append_action
from nntool.utils.json_serializable import JsonSerializableStateDecoder, JsonSerializableStateEncoder

SCHEME_OPTIONS = {
    'choices': ['scaled', 'float', 'pow2']
}

SCHEME_NAME_MAPPINGS = {
    'scaled': 'SQ8',
    'pow2': 'POW2',
    'float': 'FLOAT',
}

def valid_keys(first_arg_nodes):
    all_options = get_all_options_by_params()
    options = {}
    for node in first_arg_nodes:
        this_options = all_options.get(node.__class__, {})
        options.update(this_options)
    options['scheme'] = SCHEME_OPTIONS
    return options

def capture_shell(nntool_shell):
    return argparse.Namespace(shell=nntool_shell)

class QtuneCommand(NNToolShellBase):

    # QTUNE COMMAND
    parser_tune = NNToolArguementParser()

    def qtune_first_arg_mapper(self, nodestr):
        return self.get_node_step_or_name(nodestr, allow_comma=True)[0]

    parser_tune.add_argument(
        '--step',
        nargs='+',
        kwargs_valid_keys=valid_keys,
        kwargs_first_arg_completer=NNToolShellBase.node_step_or_name_completer(allow_comma=True),
        kwargs_first_arg_mapper=qtune_first_arg_mapper,
        action=kwargs_append_action,
        metavar="STEP KEY1=VALUE1 KEY2=VALUE2",
        help='step to tune followed by key=value pairs. Step can be specified as:\n' + NODE_SELECTOR_HELP,
        completer_method=kwargs_append_action.get_completer_method(
            valid_keys,
            kwargs_first_arg_completer=NNToolShellBase.node_step_or_name_completer(allow_comma=True),
            kwargs_first_arg_mapper=qtune_first_arg_mapper))

    parser_tune.add_argument(
        '--json',
        completer_method=Cmd.path_complete,
        help='json file to load quantization options from')

    @with_argparser(parser_tune, ns_provider=capture_shell)
    def do_qtune(self, args):
        """
Tune quantization of graph."""
        self._check_graph()
        self._check_quantized()

        def reduction(state, x):
            nodes = x[0]
            opts = x[1]
            if 'scheme' in opts:
                scheme = opts['scheme'].lower()
                if scheme not in SCHEME_NAME_MAPPINGS:
                    raise ValueError(f'invalid scheme name {opts["scheme"]}')
                opts['scheme'] = SCHEME_NAME_MAPPINGS[scheme]
            for node in nodes:
                props = state.setdefault(node.name, {})
                props.update(opts)
            return state

        if args.json:
            json_path = Path(args.json)
            if not json_path.exists() or not json_path.is_file():
                self.perror(f'{json_path} does not exist or is not a file')
                return
            graph_options, node_options = load_options(json_path)
        else:
            graph_options, node_options = {}, {}

        if args.step:
            node_options = reduce(reduction, args.step, node_options)

        quantizer = NewQuantizer.from_quantized_graph(self.G)
        quantizer.update_options(graph_options, node_options)
        quantizer.quantize()
        self.pfeedback('quantization options set')

def load_options(file_path):
    with file_path.open('r') as fp:
        save_options = json.load(fp, cls=JsonSerializableStateDecoder)
    return save_options['graph'], save_options['nodes']


class QTuneSaveCommand(NNToolShellBase):

    # QTUNESAVE COMMAND
    parser_qtune_save = NNToolArguementParser()
    parser_qtune_save.add_argument('jsonfile',
                             completer_method=Cmd.path_complete,
                             help='json file to save quantization options')

    @with_argparser(parser_qtune_save)
    def do_qtunesave(self, args):
        """
Save set quantization options.

You can manually edit quantization options in the file.
The global section contains options that will be applied to the whole graph
The nodes array contains options for each node that override the global options
Each nodes entry should be a JSON mapping with a key node_name
node_name should contain a node name or and arry with the fusion name and fusion internal
node name.
"""
        self._check_graph()
        self._check_quantized()
        save_path = Path(args.jsonfile).with_suffix('.json')
        save_options = {
            "graph": self.G.quantization.graph_options,
            "nodes": self.G.quantization.node_options
        }

        if 'scheme' not in save_options:
            save_options['scheme'] = self.G.quantization.scheme_priority[0]
        with save_path.open('w') as fp:
            json.dump(save_options, fp, cls=JsonSerializableStateEncoder, indent=2)
        self.pfeedback(f'quantization options saved to {save_path}')
