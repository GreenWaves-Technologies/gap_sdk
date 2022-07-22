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

from cmd2 import with_argparser
from nntool.importer.importer import create_graph
from nntool.interpreter.commands.open_parser import (NO_GRAPH, get_open_parser,
                                              get_state_extension,
                                              parse_open_options)
from nntool.interpreter.nntool_shell_base import NNToolShellBase
from nntool.quantization.cross_layer_range_eq import weight_equalization

from nntool.graph.matches.matchers.fuse_pad import MatchFusePad

LOG = logging.getLogger("nntool")


class OpenCommand(NNToolShellBase):
    STATE_EXTENSION = get_state_extension()

# OPEN COMMAND
    parser_open = get_open_parser()

    def __open_graph(self, args):
        graph_file = os.path.expanduser(args.nnfile)

        is_state_file, graph_file, opts = parse_open_options(
            graph_file, args, self.settings)

        if is_state_file:
            LOG.info("opening state file %s", graph_file)
            self.load_state_file(graph_file, args.orgmodel_path)
        else:

            LOG.info("opening graph file %s load_quantization = %s",
                     graph_file, opts['load_quantization'])

            self.open_graph(graph_file, **opts)

            self.settings['load_quantization'] = bool(
                opts['load_quantization'])
            if self.settings['adjust_order']:
                LOG.info("adjusting order")
                self.execute_adjust_order()
            if self.settings['weight_equalization']:
                LOG.info("equalizing weights")
                weight_equalization(
                    self.G, self.settings['equalization_threshold'])

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
        try:
            self.__open_graph(args)
        except FileNotFoundError:
            self.perror(f'{args.nnfile} not found')
            return
        self._update_prompt()
        self.py_locals['G'] = self.G

    def open_graph(self, file_path, **kwargs):
        G = create_graph(file_path, opts=kwargs)
        G.add_dimensions()
        pad_fuser = MatchFusePad()
        pad_fuser.match(G)
        G.add_dimensions()
        self.G = G
        self.graph_file = file_path
        self._reset_history()
