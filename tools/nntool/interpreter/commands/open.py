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
import os
import logging
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from utils.new_param_state import STATE_EXTENSION, load_state
from importer.importer import create_graph
from interpreter.nntool_shell_base import NNToolShellBase
from quantization.cross_layer_range_eq import weight_equalization

LOG = logging.getLogger("nntool")

NO_GRAPH = {
    'G': None,
    'graph_file': "",
    'tensor_file': ""
}

class OpenCommand(NNToolShellBase):
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
    parser_open.add_argument('-q', '--load_quantization',
                             help='load TFLite quantization information', action='store_true')
    parser_open.add_argument('-d', '--load_dequantized',
                             help='load dequantized constant values from TFLite quantized graph', action='store_true')
    parser_open.add_argument('-n', '--new',
                             help='open as new graph - keep existing graph open',
                             action='store_true')

    def __open_graph(self, graph_file, tensor_file, load_quantization, load_dequantized):

        graph_file = os.path.expanduser(graph_file)

        _, ext = os.path.splitext(graph_file)

        if ext == STATE_EXTENSION:
            LOG.info("opening state file %s", graph_file)
            self.graph_file = graph_file
            self.G, extra = load_state(graph_file, return_extra=True)
            self.settings.update(extra)
        else:
            LOG.info("opening graph file %s", graph_file)
            opts = {
                'load_tensors': True,
                'load_quantization': load_quantization,
                'load_dequantized': load_dequantized
            }

            G = create_graph(graph_file, opts=opts)
            G.add_dimensions()
            if tensor_file:
                G.load_tensors(tensor_file)
            self.G = G
            self.graph_file = graph_file
            if tensor_file is not None:
                self.tensor_file = tensor_file
            self.settings['load_quantization'] = bool(load_quantization)
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
        else:
            # reset the current graph
            self._graphs[self._graph_idx] = NO_GRAPH.copy()
        self.__open_graph(args.nnfile, args.tensor_file, args.load_quantization, args.load_dequantized)
        self._update_prompt()
        self.py_locals['G'] = self.G
