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

import logging
from copy import deepcopy
from functools import partial
from itertools import groupby

import numpy as np
from nntool.graph.dim import Dim
from nntool.graph.types import SplitNode
from nntool.graph.types.base import ComparableNodeMixin, NNEdge
from nntool.graph.types.fusions import FusionNodeBase
from nntool.utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_after, run_qtune_on_match

LOG = logging.getLogger(__name__)


def combine_stats(G, node, other_nodes):
    if not G.quantization or not G.quantization.stats:
        return
    stats = G.quantization.stats
    recs = [stats[onode.name]
            for onode in [node] + other_nodes if onode.name in stats]
    ranges = {
        "range_in": [stat['range_in'][0] for stat in recs],
        "range_out": [stat['range_out'][0] for stat in recs]
    }
    combined_range = {
        direction: [{
            "min": min(rec['min'] for rec in range_set),
            "max": max(rec['max'] for rec in range_set)
        }]
        for direction, range_set in ranges.items()
    }
    combined_range['range_in'].extend([None, None])
    for onode in other_nodes:
        if onode.name in stats:
            del stats[onode.name]
    stats[node.name].update(combined_range)


@match_name("duplicate_operations")
@description("""Removes operations that are duplicates on the same edge""")
@run_after("fuse_gap_convs")
@run_qtune_on_match
@groups('symmetric', 'scaled')
class MatchDuplicateOperations(Matcher):
    def __init__(self, identity: str = None, limit_to_dest_classes=None):
        super().__init__(identity)
        self._limit_to_dest_classes = limit_to_dest_classes

    def _match(self, G: GraphView, **kwargs):
        # if G.quantization:
        #     LOG.warning(
        #         'duplicate_operations does not handle quantized graphs')
        #     return False

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
            if self._limit_to_dest_classes:
                same_source_edges = list(
                    filter(
                        lambda edges: all(isinstance(
                            edge.to_node, self._limit_to_dest_classes) for edge in edges),
                        same_source_edges))
            same_dest_edges = []
            same_dest_group_edges = []

            for same_source_edge in same_source_edges:
                same_source_edge = [edge for edge in same_source_edge if isinstance(
                    edge.to_node, ComparableNodeMixin)]
                while same_source_edge:
                    first_edge = same_source_edge.pop(0)

                    others = list(
                        filter(
                            partial(
                                lambda x, y: (x.to_node != y.to_node and
                                              y.to_node.is_same_operation_as(G, x.to_node)),
                                first_edge),
                            same_source_edge))
                    if others:
                        same_dest_edges.append(tuple([first_edge] + others))
                        for other in others:
                            same_source_edge.remove(other)
                        continue

                    other_groups = list(
                        filter(
                            partial(
                                lambda x, y: (x.to_node != y.to_node and
                                              y.to_node.can_be_grouped_with(G, x.to_node)),
                                first_edge),
                            same_source_edge))
                    if other_groups:
                        same_dest_group_edges.append(
                            tuple([first_edge] + other_groups))
                        for other in other_groups:
                            same_source_edge.remove(other)

            # all are multiple edges that go to something comparable
            while same_dest_edges:
                edge_set = same_dest_edges.pop(0)
                keep_node = edge_set[0].to_node
                other_edge_sets = [edges for edges in same_dest_edges if any(
                    edge.to_node == keep_node for edge in edges)]
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
                            G.add_edge(
                                NNEdge(from_node=keep_node, to_node=out_edge.to_node, to_idx=out_edge.to_idx))
                LOG.info(
                    f'removed duplicates {",".join(node.name for node in nodes_to_delete)} to {keep_node.name}')
                for node in nodes_to_delete:
                    G.remove(node)


            for edge_set in same_dest_group_edges:
                modified_graph = True
                found_more = True
                # we will merge all the convolutions into one

                first_edge = edge_set[0]
                first_node = first_edge.to_node
                other_nodes = [edge.to_node for edge in edge_set[1::]]

                in_edges = G.indexed_in_edges(first_node.name)
                first_filter = first_node.filter_dim
                weights_node = in_edges[1].from_node
                biases_node = in_edges[2].from_node

                out_shape = deepcopy(first_node.out_dims[0])
                # the new number of output channels is the combination of all the output channels
                out_cs = [node.out_dims[0].c for node in [first_node] + other_nodes]
                out_shape.c = sum(out_cs)

                # create a split after the first node splitting on channel axis
                # The convs can have different amounts of output channels
                act_slices, out_shapes, axis = SplitNode.get_splits(
                    out_shape, out_shape.get_order_idx('c'), splits=out_cs)
                split1 = SplitNode(G.unique_name(
                    f'{first_node.name}_split'), act_slices=act_slices, out_shapes=out_shapes, axis=axis)
                out_num = 0
                # first node out edge goes to split
                out_edges = G.out_edges(first_node.name)
                for edge in out_edges:
                    G.remove_edge(edge)
                    G.add_edge(NNEdge(from_node=split1, from_idx=out_num,
                                      to_node=edge.to_node, to_idx=edge.to_idx))
                G.add_edge(NNEdge(from_node=first_node, to_node=split1))

                combine_stats(G, first_node, other_nodes)
                self.remove_quantization(
                    G, first_node, weights_node, biases_node)
                weights_node.dqvalue = weights_node.dqvalue
                biases_node.dqvalue = biases_node.dqvalue

                # first split output goes to original output
                for other_node in other_nodes:
                    out_num += 1
                    in_edges = G.indexed_in_edges(other_node.name)
                    weights_other = in_edges[1].from_node
                    biases_other = in_edges[2].from_node

                    # merge the weights and biases down output channel
                    # rather strange but has side effect of clearing any weight/bias quantization
                    if weights_other.qtype:
                        weights_other.dqvalue = weights_other.dqvalue
                    if biases_other.qtype:
                        biases_other.dqvalue = biases_other.dqvalue

                    weights_node.value = np.concatenate(
                        (weights_node.value, weights_other.value), axis=first_filter.get_order_idx('out_c'))
                    weights_node.dims = Dim.unnamed(weights_node.value.shape)
                    biases_node.value = np.concatenate(
                        (biases_node.value, biases_other.value))
                    biases_node.dims = Dim.unnamed(biases_node.value.shape)
                    first_filter.out_c += other_node.filter_dim.out_c
                    # wire edge from split
                    out_edges = G.out_edges(other_node.name)
                    G.remove(other_node)
                    G.remove(weights_other)
                    G.remove(biases_other)
                    for edge in out_edges:
                        G.add_edge(NNEdge(from_node=split1, from_idx=out_num,
                                          to_node=edge.to_node, to_idx=edge.to_idx))
                other_node_names = [node.name for node in other_nodes]
                LOG.info(
                    f'merged convolutions {",".join(other_node_names)} into {first_node.name}')
            if not found_more:
                break

        return modified_graph

    def remove_quantization(self, G, *nodes):
        if G.quantization:
            for nid in [node.name for node in nodes]:
                if nid in G.quantization:
                    del G.quantization[nid]
