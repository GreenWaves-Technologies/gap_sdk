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
from graph.types import Parameters

from cmd2 import Cmd2ArgumentParser, with_argparser

from interpreter.nntool_shell_base import NNToolShellBase

from utils.node_id import NodeId


class RemoveCommand(NNToolShellBase):
    def nodes_choices(self):
        if self.G is None:
            return []
        return [node.name for node in self.G.nodes()]

    # resizer COMMAND
    parser_remove = Cmd2ArgumentParser(
        "Removes all the edges and nodes between two node. Will only work if nodes do not affect shape of tensor.")
    parser_remove.add_argument('nodes',
                               nargs=2,
                               choices_method=nodes_choices,
                               help='node from and to to remove.')

    @with_argparser(parser_remove)
    def do_remove(self, args: argparse.Namespace):
        """Removes all the edges and nodes between two node. Will only work if nodes do not affect shape of tensor."""
        self._check_graph()
        if any(node not in self.G for node in args.nodes):
            self.perror("node not found in graph")
            return
        node_from = self.G[args.nodes[0]]
        node_to = self.G[args.nodes[1]]
        edge_from = self.G.out_edges(node_from.name)
        edge_to = self.G.in_edges(node_to.name)
        if len(edge_from) != 1:
            self.perror("node from has more than one out edge")
            return
        if len(edge_to) != 1:
            self.perror("node to has more than one in edge")
            return
        edge_from = edge_from[0]
        edge_to = edge_to[0]
        if edge_from == edge_to:
            self.perror("nodes are directly connected")
            return
        try:
            edges = paths_from(self.G, node_from, node_to)
        except ValueError as ex:
            self.perror(ex)
            return
        edges.remove(edge_from)
        remove_nodes(self.G, edges)
        edge_from.to_node = edge_to.to_node
        edge_from.to_idx = edge_to.to_idx
        self.G.add_edge(edge_from)
        self.G.add_dimensions()

def remove_nodes(G, edges):
    for edge in edges:
        G.remove(edge.from_node)

def up_edges(G, cur_node, exclude=None):
    in_edges = set(G.in_edges(cur_node.name))
    if exclude:
        in_edges -= exclude
    if len(in_edges) > 1:
        raise ValueError()
    if in_edges:
        return in_edges | up_edges(G, next(iter(in_edges)).from_node)
    return set()

def paths_from(G, node_from, node_to):
    if isinstance(node_from, Parameters):
        in_shape = None
        this_up = set()
    else:
        edge_from = node_from
        node_from = edge_from.to_node
        if node_from == node_to:
            return set([edge_from])
        this_up = up_edges(G, node_from, exclude=set([edge_from]))
        in_shape = node_from.in_dims[edge_from.to_idx].shape
    res = set()
    for edge in G.out_edges(node_from.name):
        if in_shape is not None and edge.from_node.out_dims[edge.from_idx].shape != in_shape:
            raise ValueError("%s modifies shape of tensor"%(edge.from_node.name))
        this_res = paths_from(G, edge, node_to)
        res |= set([edge]) | this_res
    if res:
        return this_up | res
    raise ValueError("%s has no outputs and is not the to node"%(node_from.name))


