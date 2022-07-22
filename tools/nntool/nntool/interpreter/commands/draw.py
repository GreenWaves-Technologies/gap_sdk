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

from cmd2 import Cmd, with_argparser
from nntool.interpreter.nntool_shell_base import (NODE_SELECTOR_HELP,
                                           NNToolArguementParser,
                                           NNToolShellBase, no_history)
from nntool.reports.draw_graph_reporter import DrawGraphReporter

from nntool.graph.types import ConstantInputNode


class DrawCommand(NNToolShellBase):
    # Draw COMMAND
    parser_draw = NNToolArguementParser("display graph")
    parser_draw.add_argument('step', nargs=(0, 1),
                             help='step to show or nothing to show all.' + NODE_SELECTOR_HELP,
                             completer_method=NNToolShellBase.node_step_or_name_completer())
    parser_draw.add_argument('-s', '--show_constants', action='store_true',
                             help='Show constant parameters nodes')
    parser_draw.add_argument('-a', '--all_edges',
                             action='store_true',
                             help='Show labels on all inputs and outputs even '
                             'if destination or source node is not selected')
    parser_draw.add_argument('-F', '--fusions',
                             action='store_true',
                             help='Show fusions as subgraphs')
    parser_draw.add_argument('--expressions',
                             choices=['quantized', 'unquantized'],
                             help='show expressions as subgraphs')
    parser_draw.add_argument('-y', '--anonymise',
                             action='store_true',
                             help='anonymise the graph operations as much as possible while keeping debug info')
    parser_draw.add_argument('-q', '--quant_labels',
                             action='store_true',
                             help='label edges with quantization not shape')
    parser_draw.add_argument('-f', '--file',
                             completer_method=Cmd.path_complete,
                             help='save the drawn graph to a file',
                             metavar="GRAPH_FILE")

    @with_argparser(parser_draw)
    @no_history
    def do_draw(self, args):
        """
Render the graph to a simple image."""
        self._check_graph()

        if args.step:
            nodes, _ = self.get_node_step_or_name(args.step)
            if not nodes:
                self.do_help('show')
                return
        else:
            nodes = self.G.nodes()

        if not args.show_constants:
            if nodes:
                nodes = set([node for node in nodes if not isinstance(
                    node, ConstantInputNode)])
            else:
                nodes = set([node for node in self.G.nodes()
                             if not isinstance(node, ConstantInputNode)])
        if not nodes:
            self.perror('no nodes selected')
            self.do_help('draw')
            return
        DrawGraphReporter().report(self.G, nodes=nodes, all_dims=args.all_edges,
                                   filename=args.file, view=args.file is None,
                                   anonymise=args.anonymise, expressions=args.expressions,
                                   quant_labels=args.quant_labels,
                                   fusions=args.fusions)
