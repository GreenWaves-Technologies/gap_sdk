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

import argparse
import logging
import os

from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from importer.common.handler_options import HandlerOptions
from importer.importer import create_graph
from interpreter.nntool_shell_base import NNToolShellBase
from quantization.cross_layer_range_eq import weight_equalization

LOG = logging.getLogger("nntool")

NO_GRAPH = {
    'G': None,
    'graph_file': "",
    'tensor_file': ""
}

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

class OpenCommand(NNToolShellBase):
    STATE_EXTENSION = '.json'

# OPEN COMMAND
    parser_open = Cmd2ArgumentParser("open a graph file")
    parser_open.add_argument('nnfile',
                             completer_method=Cmd.path_complete,
                             help='graph or state file',
                             metavar="INPUT_GRAPH or STATE_FILE")
    parser_open.add_argument('-n', '--new',
                             help='open as new graph - keep existing graph open',
                             action='store_true')
    add_open_options(parser_open)

    def __get_opts(self, args):
        return {k: getattr(args, k) if (option['val_type'] != bool or not option['default'])
                else not getattr(args, f'no_{option["name"]}')
                for k, option in HandlerOptions.get_all_handler_options().items()}

    def __open_graph(self, args):

        graph_file = os.path.expanduser(args.nnfile)

        _, ext = os.path.splitext(graph_file)
        ext = ext.lower()

        if ext == self.STATE_EXTENSION:
            LOG.info("opening state file %s", graph_file)
            self.load_state_file(graph_file)
        else:
            opts = self.__get_opts(args)

            LOG.info("opening graph file %s load_quantization = %s",
                     graph_file, opts['load_quantization'])

            G = create_graph(graph_file, opts=opts)
            G.add_dimensions()
            self.G = G
            self.graph_file = graph_file
            self._reset_history()

            self.settings['load_quantization'] = bool(opts['load_quantization'])
            if self.settings['adjust_order']:
                LOG.info("adjusting order")
                self.execute_adjust_order()
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
            self._reset_history()
        else:
            # reset the current graph
            self._graphs[self._graph_idx] = NO_GRAPH.copy()
        self.__open_graph(args)
        self._update_prompt()
        self.py_locals['G'] = self.G
