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

from graph.dim import Dim
import logging
from copy import deepcopy
from functools import partial
from itertools import groupby

import numpy as np
from graph.types import SplitParameters
from graph.types.base import ComparableParameters, NNEdge
from utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_before

LOG = logging.getLogger("nntool." + __name__)

@match_name("match_duplicate_operations")
@description("""Removes operations that are duplicates on the same edge""")
@run_before("*")
@groups('symmetric', 'scaled')
class MatchDuplicateOperations(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        if G.quantization:
            LOG.warning(
                'match_duplicate_operations does not handle quantized graphs')
            return False

        def same_source_edge_fn(x):
            return f"{x.from_node.__hash__()}##{x.from_idx}"

        def same_dest_edge(x):
            return f"{x.to_node.__hash__()}##{x.to_idx}"

        modified_graph = False
        while True:
            found_more = False
            same_source_edges = [list(edge_list) for _, edge_list in
                                 groupby(sorted(G.edges(), key=same_source_edge_fn), same_source_edge_fn)]
            # all have the same origin
            same_source_edges = [elem for elem in same_source_edges
                                 if len(elem) > 1]
            same_dest_edges = []
            same_dest_group_edges = []

            for same_source_edge in same_source_edges:
                same_source_edge = [edge for edge in same_source_edge if isinstance(
                    edge.to_node, ComparableParameters)]
                while same_source_edge:
                    first = same_source_edge.pop(0)

                    others = list(filter(partial(lambda x, y: y.to_node.is_same_operation_as(G,
                        x.to_node), first), same_source_edge))
                    if others:
                        same_dest_edges.append(tuple([first] + others))
                        for other in others:
                            same_source_edge.remove(other)
                        continue

                    other_groups = list(filter(partial(lambda x, y: y.to_node.can_be_grouped_with(
                        x.to_node), first), same_source_edge))
                    if other_groups:
                        same_dest_group_edges.append(
                            tuple([first] + other_groups))
                        for other in other_groups:
                            same_source_edge.remove(other)

            # all are multiple edges that go to something comparable
            while same_dest_edges:
                edge_set = same_dest_edges.pop(0)
                keep_node = edge_set[0].to_node
                other_edge_sets = [edges for edges in same_dest_edges if any(edge.to_node == keep_node for edge in edges)]
                for other_edge_set in other_edge_sets:
                    same_dest_edges.remove(other_edge_set)
                
                nodes_to_delete = set()
                for edge_set in [edge_set] + other_edge_sets:
                    for edge in edge_set:
                        other_node = edge.to_node
                        if other_node == keep_node or other_node in nodes_to_delete:
                            continue
                        nodes_to_delete.add(other_node)
                        for out_edge in G.out_edges(other_node):
                            G.add_edge(NNEdge(from_node=keep_node, to_node=out_edge.to_node, to_idx=out_edge.to_idx))
                LOG.info(
                    f'removed duplicates {",".join(node.name for node in nodes_to_delete)} to {keep_node.name}')
                for node in nodes_to_delete:
                    G.remove(node)
                        
            
            # # all are multiple edges that go to something comparable

            # for edge_set in same_dest_edges:
            #     modified_graph = True
            #     found_more = True
            #     first = edge_set[0]
            #     first_node = first.to_node
            #     dup_nodes = []
            #     for other in edge_set[1::]:
            #         dest_node = other.to_node
            #         dup_nodes.append(dest_node.name)
            #         out_edges = G.out_edges(dest_node.name)
            #         G.remove(dest_node)
            #         for out_edge in out_edges:
            #             G.add_edge(NNEdge(from_node=first_node, to_node=out_edge.to_node,
            #                               from_idx=out_edge.from_idx, to_idx=out_edge.to_idx))
            #     LOG.info(
            #         f'removed duplicates {",".join(dup_nodes)} to {first_node.name}')

            for edge_set in same_dest_group_edges:
                modified_graph = True
                found_more = True
                # we will merge all the convolutions into one
                first = edge_set[0]
                first_node = first.to_node
                in_edges = G.indexed_in_edges(first_node.name)
                first_filter = first_node.filter
                weights_node = in_edges[1].from_node
                biases_node = in_edges[2].from_node
                dup_nodes = []
                num_convs = len(edge_set)
                out_shape = deepcopy(first_node.out_dims[0])
                out_shape.c *= num_convs
                # create a split after the first node splitting on channel axis
                act_slices, out_shapes, axis = SplitParameters.get_splits(
                    out_shape, out_shape.get_order_idx('c'), num_splits=num_convs)
                split1 = SplitParameters(G.unique_name(
                    f'{first_node.name}_split'), act_slices=act_slices, out_shapes=out_shapes, axis=axis)
                out_num = 0
                # first node out edge goes to split
                out_edges = G.out_edges(first_node.name)
                for edge in out_edges:
                    G.remove_edge(edge)
                    G.add_edge(NNEdge(from_node=split1, from_idx=out_num, to_node=edge.to_node, to_idx=edge.to_idx))
                G.add_edge(NNEdge(from_node=first_node, to_node=split1))
                # first split output goes to original output
                for other in edge_set[1::]:
                    out_num += 1
                    node_other = other.to_node
                    dup_nodes.append(node_other.name)
                    in_edges = G.indexed_in_edges(node_other.name)
                    weights_other = in_edges[1].from_node
                    biases_other = in_edges[2].from_node
                    # merge the weights and biases diwn output channel
                    weights_node.value = np.concatenate(
                        (weights_node.value, weights_other.value), axis=first_filter.get_order_idx('out_c'))
                    weights_node.dims = Dim.unnamed(weights_node.value.shape)
                    biases_node.value = np.concatenate(
                        (biases_node.value, biases_other.value))
                    biases_node.dims = Dim.unnamed(biases_node.value.shape)
                    first_filter.out_c += node_other.filter.out_c
                    # wire edge from split
                    out_edges = G.out_edges(node_other.name)
                    G.remove(node_other)
                    G.remove(weights_other)
                    G.remove(biases_other)
                    for edge in out_edges:
                        G.add_edge(NNEdge(from_node=split1, from_idx=out_num, to_node=edge.to_node, to_idx=edge.to_idx))
                LOG.info(
                    f'merged convolutions {",".join(dup_nodes)} into {first_node.name}')
            if not found_more:
                break

        if set_identity:
            self.set_identity(G)

        return modified_graph
