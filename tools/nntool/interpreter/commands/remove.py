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

from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase

from graph.types import Parameters
from graph.types.base import NNEdge


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

    @with_argparser(parser_remove)
    def do_remove(self, args: argparse.Namespace):
        """Removes all the edges and nodes between two node. Will only work if nodes do not affect shape of tensor."""
        self._check_graph()
        if any(node not in self.G for node in args.nodes):
            self.perror("node not found in graph")
            return
        node_from = self.G[args.nodes[0]]
        if len(args.nodes) == 1:
            if args.up:
                out_edges = self.G.indexed_out_edges(node_from.name)
                self.remove_to_input(node_from)
                for idx, edge_group in enumerate(out_edges):
                    in_node = self.G.add_input(node_from.out_dims[idx])
                    self.pfeedback(f'adding input {in_node.name}')
                    for edge in edge_group:
                        self.G.add_edge(NNEdge(from_node=in_node,
                                               to_idx=edge.to_idx,
                                               to_node=edge.to_node))
            else:
                in_edges = self.G.in_edges(node_from.name)
                self.remove_to_output(node_from)
                for edge in in_edges:
                    out_node = self.G.add_output()
                    self.pfeedback(f'adding output {out_node.name}')
                    self.G.add_edge(NNEdge(from_node=edge.from_node,
                                           from_idx=edge.from_idx, to_node=out_node))
        else:
            node_to = self.G[args.nodes[1]]
            edge_from = self.G.out_edges(node_from.name)
            edge_to = self.G.in_edges(node_to.name)
            if len(edge_from) != 1:
                self.perror("node from has more than one out edge")
                return
            edge_from = edge_from[0]
            if len(edge_to) != 1:
                edge_to = self.find_to(edge_from, set(edge_to))
                if edge_to is None:
                    self.perror("nodes don't seem to be connected")
                    return
            else:
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

    def find_to(self, edge_from, edges):
        if edge_from in edges:
            return edge_from
        for edge in self.G.out_edges(edge_from.to_node.name):
            res = self.find_to(edge, edges)
            if res:
                return res
        return None

    def remove_to_output(self, start_node, visited_edges=None):
        if visited_edges is None:
            visited_edges = set()
        for edge in self.G.out_edges(start_node.name):
            visited_edges.add(edge)
            # don't continue here unless we have visited all the in edges to the node below
            if not set(self.G.in_edges(edge.to_node.name)).issubset(visited_edges):
                continue
            self.remove_to_output(edge.to_node, visited_edges=visited_edges)
        self.pfeedback(f'removing node {start_node.name}')
        self.G.remove(start_node)

    def remove_to_input(self, start_node, visited=None, visited_edges=None):
        if visited is None:
            visited = set()
            visited_edges = set()
        for edge in self.G.in_edges(start_node.name):
            visited_edges.add(edge)
            # don't continue here unless we have visited all the out edges to the node above
            if not set(self.G.out_edges(edge.from_node.name)).issubset(visited_edges):
                continue
            self.remove_to_input(edge.from_node, visited_edges=visited_edges)
        self.pfeedback(f'removing node {start_node.name}')
        self.G.remove(start_node)


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
            raise ValueError("%s modifies shape of tensor" %
                             (edge.from_node.name))
        this_res = paths_from(G, edge, node_to)
        res |= set([edge]) | this_res
    if res:
        return this_up | res
    raise ValueError("%s has no outputs and is not the to node" %
                     (node_from.name))
