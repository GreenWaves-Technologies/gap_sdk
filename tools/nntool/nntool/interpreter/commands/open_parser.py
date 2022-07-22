# Copyright (C) 2020  GreenWaves Technologies, SAS

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
import os

import numpy as np
from cmd2 import Cmd, Cmd2ArgumentParser
from nntool.importer.common.handler_options import HandlerOptions
from nntool.utils.argparse_kwargs import kwargs_append_action
from nntool.importer.onnx.handlers.backend import *
from nntool.importer.tflite2.handlers.backend import *

STATE_EXTENSION = '.json'

NO_GRAPH = {
    'G': None,
    'graph_file': "",
    'tensor_file': ""
}

valid_keys = {
    '*': {
        'type': int
    },
}

valid_keys_arrays = {
    '*': {
        'type': lambda x: eval(f'np.array({x})', {}, {'np': np})
    },
}


def get_open_parser():
    parser_open = Cmd2ArgumentParser("open a graph file")
    parser_open.add_argument('nnfile',
                             completer_method=Cmd.path_complete,
                             help='graph or state file',
                             metavar="INPUT_GRAPH or STATE_FILE")
    parser_open.add_argument('--orgmodel_path',
                             completer_method=Cmd.path_complete,
                             help='graph file', default=None,
                             metavar="INPUT_GRAPH or STATE_FILE")
    parser_open.add_argument('-n', '--new',
                             help='open as new graph - keep existing graph open',
                             action='store_true')
    add_open_options(parser_open)
    return parser_open


def add_open_options(parser):
    # add all the options defined by import handlers
    # if a boolean option has a default of True then add a --no-option-name
    # flag option
    for option in HandlerOptions.get_all_handler_options().values():
        add_kwargs = {}
        if option['val_type'] == bool and option['default']:
            add_args = [f'--no_{option["name"]}']
            add_kwargs['help'] = f'disable option {option["name"]}: {option["desc"]}'
        elif option['shortcut']:
            add_args = [f'-{option["shortcut"]}', f'--{option["name"]}']
            add_kwargs['help'] = option['desc']
        else:
            add_args = [f'--{option["name"]}']
            add_kwargs['help'] = option['desc']
        if option['val_type'] == bool:
            add_kwargs['action'] = 'store_true'
        else:
            add_kwargs['type'] = option['val_type']
            add_kwargs['default'] = option['default']

        parser.add_argument(*add_args, **add_kwargs)
    parser.add_argument('--subs',
                        nargs='*',
                        action=kwargs_append_action,
                        kwargs_valid_keys=valid_keys,
                        metavar="KEY=VALUE",
                        help='values to substitute for unknown dimensions in the form var=val',
                        )
    parser.add_argument('--input_shapes',
                        nargs='*',
                        action=kwargs_append_action,
                        kwargs_valid_keys=valid_keys_arrays,
                        metavar="KEY=ARRAY_VALUE",
                        help=('shape to override input shape with. should be in the form input_name=[dim, dim, dim, etc]'
                              'where dim is either an integer or None if this axis should not be overriden'),
                        )
    parser.add_argument('--fixed_inputs',
                        nargs='*',
                        action=kwargs_append_action,
                        kwargs_valid_keys=valid_keys_arrays,
                        metavar="KEY=ARRAY_VALUE",
                        help='inputs to fix to an array value. should be in the form input_name=array of arrays ...'
                        )
    parser.add_argument('--out_ranges',
                        completer_method=Cmd.path_complete,
                        help='path to json file with: {"node_name": {"range": [min, max], "n_bits": 8, "per_channel": None, "dtype": np.int8}}  out range to override particular qrecs'
                        )


def parse_open_options(file_path, args, settings):
    _, ext = os.path.splitext(file_path)
    ext = ext.lower()

    if ext == STATE_EXTENSION:
        return True, file_path, {'orgmodel_path': args.orgmodel_path}
    else:
        opts = {k: getattr(args, k) if (option['val_type'] != bool or not option['default'])
                else not getattr(args, f'no_{option["name"]}')
                for k, option in HandlerOptions.get_all_handler_options().items()}

        opts['substitutions'] = args.subs
        opts['input_shapes'] = args.input_shapes
        opts['fixed_inputs'] = args.fixed_inputs
        if args.out_ranges:
            with open(args.out_ranges) as f:
                opts['ranges_dict'] = json.load(f)

        opts['anonymise'] = settings['anonymise']
        return False, file_path, opts

def get_state_extension():
    return STATE_EXTENSION
