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
from functools import reduce
from itertools import chain, groupby

from cmd2 import Cmd2ArgumentParser, with_argparser
from nntool.interpreter.nntool_shell_base import NNToolShellBase

from nntool.graph.types import ReshapeNode, InputNode, OutputNode, ConstantInputNode
from nntool.graph.types.base import NNEdge
from nntool.quantization.new_qrec import QRec


class RemoveCommand(NNToolShellBase):
    def nodes_choices(self):
        if self.G is None:
            return []
        return [node.name for node in self.G.nodes()]

    # resizer COMMAND
    parser_remove = Cmd2ArgumentParser(
        "Removes all the edges and nodes between two node. Will only work if nodes do not affect shape of tensor."
        "If one node is specified this will remove all nodes following it and the node specified. New output nodes will "
        "be created on all the inputs to the node specified. Specifying up does the remove upwards and creates inputs.")
    parser_remove.add_argument('nodes',
                               nargs=(1, 2),
                               choices_method=nodes_choices,
                               help='node from and to to remove.')
    parser_remove.add_argument('-u', '--up',
                               action='store_true',
                               help='when one node is specified remove it and everything above it')
    parser_remove.add_argument('--leave',
                               action='store_true',
                               help='when one node is specified only remove what is above or below and not the node itself')
    parser_remove.add_argument('--no_check',
                               action='store_true',
                               help="don't check that the graph structure is damaged")

    @with_argparser(parser_remove)
    def do_remove(self, args: argparse.Namespace):
        """Removes all the edges and nodes between two node. Will only work if nodes do not affect shape of tensor."""
        self._check_graph()
        for node in args.nodes:
            if node not in self.G:
                self.perror(f"node {node} not found in graph")
                return
        node_from = self.G[args.nodes[0]]
        node_to = self.G[args.nodes[1]] if len(args.nodes) > 1 else None
        self.G.remove_nodes(node_from, node_to, up=args.up, leave=args.leave, no_check=args.no_check)
