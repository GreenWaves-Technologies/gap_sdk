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

from graph.types import ReshapeParameters, InputParameters, OutputParameters, ConstantInputParameters
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
                nodes_above = self.G.nodes_above(node_from)
                out_edges = self.G.indexed_out_edges(node_from)
                nodes_above.add(node_from)
                input_names = sorted(
                    [node.name for node in nodes_above if isinstance(node, InputParameters)])
                self.G.remove_all(nodes_above | {node_from})
                for idx, edge_group in enumerate(out_edges):
                    name = input_names.pop(0) if input_names else None
                    in_node = self.G.add_input(
                        node_from.out_dims[idx], name=name)
                    self.pfeedback(f'adding input {in_node.name}')
                    for edge in edge_group:
                        self.G.add_edge(NNEdge(from_node=in_node,
                                               to_idx=edge.to_idx,
                                               to_node=edge.to_node))
            else:
                nodes_below = self.G.nodes_below(node_from)
                for node in list(nodes_below):
                    nodes_below.update(edge.from_node for edge in self.G.in_edges(node)
                                       if isinstance(edge.from_node, ConstantInputParameters))
                if self.G.is_vertex_cut(nodes_below):
                    self.perror(
                        f'removing everything below {node_from.name} would split the graph which is not permitted')
                    return
                nodes_below.add(node_from)
                in_edges = self.G.in_edges(node_from.name)
                output_names = sorted(
                    [node.name for node in nodes_below if isinstance(node, OutputParameters)])
                self.G.remove_all(nodes_below)
                for edge in in_edges:
                    name = output_names.pop(0) if output_names else None
                    out_node = self.G.add_output(name=name)
                    self.pfeedback(f'adding output {out_node.name}')
                    self.G.add_edge(NNEdge(from_node=edge.from_node,
                                           from_idx=edge.from_idx, to_node=out_node))
        else:
            node_to = self.G[args.nodes[1]]
            nodes_between = self.G.nodes_between(node_from, node_to)
            if not nodes_between:
                self.perror(
                    f'there are no nodes between {node_from.name} and {node_to.name}')
                return
            if not self.G.nodes_between_in(node_from, node_to, nodes_between):
                self.perror(
                    f'all paths from {node_from.name} must lead to {node_to.name}')
                return

            edges_from = self.G.indexed_out_edges(node_from)
            edges_to = self.G.indexed_in_edges(node_to.name)
            if len(edges_from) != len(edges_to):
                self.perror(
                    f"{node_from.name} has a different number of outputs than {node_to.name}'s inputs")
                return
            for idx, _ in enumerate(edges_from):
                if node_from.out_dims[idx].size() != node_to.in_dims[idx].size():
                    self.perror(
                        f"{node_from.name} output {idx} has a different size to {node_to.name}'s input")
                    return
            self.G.remove_all(nodes_between)
            for idx, _ in enumerate(edges_from):
                if node_from.out_dims[idx].shape != node_to.in_dims[idx].shape:
                    reshape = ReshapeParameters(
                        self.G.unique_name(f'{node_from.name}_reshape{idx}'),
                        old_shape=node_from.out_dims[idx],
                        shape=node_to.in_dims[idx])
                    self.G.add_edge(NNEdge(from_node=node_from,
                                           from_idx=idx, to_node=reshape))
                    self.G.add_edge(
                        NNEdge(from_node=reshape, to_node=node_to, to_idx=idx))
                else:
                    self.G.add_edge(NNEdge(from_node=node_from,
                                           from_idx=idx, to_node=node_to, to_idx=idx))
        self.G.add_dimensions()
