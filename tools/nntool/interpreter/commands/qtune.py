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
from interpreter.nntool_shell_base import (NODE_SELECTOR_HELP,
                                           NNToolArguementParser,
                                           NNToolShellBase)
from quantization.handlers_helpers import get_all_options_by_params
from quantization.quantizer.new_quantizer import NewQuantizer
from utils.argparse_kwargs import kwargs_append_action
from utils.json_serializable import JsonSerializableStateDecoder, JsonSerializableStateEncoder
from utils.node_id import NodeId

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
                props = state.setdefault(NodeId(node), {})
                props.update(opts)
            return state

        if args.json:
            json_path = Path(args.json)
            if not json_path.exists() or not json_path.is_file():
                self.perror(f'{json_path} does not exist or is not a file')
                return
            options = load_options(json_path)
        else:
            options = {}

        if args.step:
            options = reduce(reduction, args.step, options)

        quantizer = NewQuantizer(self.G)
        quantizer.options.update(options)
        quantizer.quantize()
        self.pfeedback('quantization options set')

def load_options(file_path):
    with file_path.open('r') as fp:
        save_options = json.load(fp, cls=JsonSerializableStateDecoder)
        options = save_options['global']
        for node_opt in save_options['nodes']:
            if 'node_name' not in node_opt:
                raise ValueError('node option missing node id')
            options[NodeId(node_opt['node_name'])] = {opt: val for opt, val in node_opt.items() if opt != "node_name"}
    return options


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
            "global": {},
            "nodes": []
        }
        for optid, opt in self.G.quantization.options.items():
            if isinstance(optid, NodeId):
                opt = opt.copy()
                if 'qtype_ind' in opt:
                    del opt['qtype_ind']
                if opt:
                    opt['node_name'] = optid.id[0] if not optid.id[1] else optid
                    save_options['nodes'].append(opt)
            else:
                save_options['global'][optid] = opt

        if 'scheme' not in save_options:
            save_options['scheme'] = self.G.quantization.scheme_priority[0]
        with save_path.open('w') as fp:
            json.dump(save_options, fp, cls=JsonSerializableStateEncoder, indent=2)
        self.pfeedback(f'quantization options saved to {save_path}')
