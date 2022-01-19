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

from graph.dim import Dim
from graph.types import ConcatParameters, NNEdge, SplitParameters
from graph.types.others import (CopyParameters, NoOPParameters,
                                ReshapeParameters, TransposeParameters)
from utils.graph import GraphView
from utils.node_id import NodeId

from ..match_utils import search_down
from ..matcher import Matcher, description, groups, match_name, run_before

LOG = logging.getLogger("nntool." + __name__)


@groups('*')
@match_name("concat_split")
@description("removes concat/split pair where all in edges on the concat match the out edges on the split")
class ConcatSplitMatch(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        has_modified_graph = False
        for split_node in set([node for node in G.nodes() if isinstance(node, SplitParameters)]):
            in_edges = G.in_edges(split_node.name)
            if len(in_edges) > 1:
                continue
            in_edge = in_edges[0]
            if not isinstance(in_edge.from_node, ConcatParameters):
                continue
            concat_node = in_edge.from_node
            if len(G.out_edges(concat_node.name)) > 1:
                continue
            if concat_node.axis != split_node.axis:
                continue
            axis = concat_node.axis
            split_out_sizes = [out_shape[axis]
                               for out_shape in split_node.out_shapes]
            if len(split_out_sizes) != len(concat_node.in_dims):
                continue
            if not all(split_out_sizes[idx] == in_dim.shape[axis] for idx, in_dim in enumerate(concat_node.in_dims)):
                continue
            has_modified_graph = True
            LOG.info("removing unnecessary concat/split pair %s/%s",
                     concat_node.name, split_node.name)
            concat_in_edges = G.indexed_in_edges(concat_node.name)
            split_out_edges = G.indexed_out_edges(split_node.name)
            G.remove(split_node)
            G.remove(concat_node)
            for idx, in_edge in enumerate(concat_in_edges):
                for out_edge in split_out_edges[idx]:
                    G.add_edge(NNEdge(from_node=in_edge.from_node, from_idx=in_edge.from_idx,
                                      to_node=out_edge.to_node, to_idx=out_edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return has_modified_graph


def reduce_slices(slices, shapes):
    res_slice = []
    res_shape = []
    for slice_axis, shape_axis in zip(zip(*slices), zip(*shapes)):
        if slice_axis[0] == slice_axis[1]:
            res_slice.append(slice_axis[0])
            res_shape.append(shape_axis[0])
        else:
            res_slice.append(
                (slice_axis[0][0],
                 slice_axis[-1][1],
                 slice_axis[0][2]))
            res_shape.append(sum(shape_axis))
    return res_slice, res_shape


def remove_edges(G, edges):
    if not edges:
        return
    edges = edges.copy()
    while len(edges) > 1:
        edge = edges.pop(0)
        G.remove(edge.to_node)
        if G.quantization:
            nid = NodeId(edge.to_node)
            if nid in G.quantization:
                del G.quantization[nid]
    try:
        G.remove_edge(edges[0])  # @IgnoreException
    except KeyError:
        pass


@groups('*')
@match_name("split_concat")
@run_before('remove_noops', 'remove_copies')
@description("removes splits that go to concats where all the out edges of the split are in sequence in the concat")
class SplitConcatMatch(Matcher):
    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        edge_groups = []
        for node in G.nodes(node_classes=SplitParameters):
            cur_group = None
            for out_edge_bundle in G.indexed_out_edges(node):
                if len(out_edge_bundle) == 1:
                    out_edge = out_edge_bundle[0]
                    concat_node_edges = search_down(
                        G, out_edge, ConcatParameters,
                        can_pass=(CopyParameters, NoOPParameters,
                                  ReshapeParameters),
                        can_pass_fn=lambda _, node: isinstance(node, TransposeParameters) and node.does_nothing)
                    if concat_node_edges:
                        if cur_group:
                            this_concat_edge = concat_node_edges[-1]
                            last_concat_edge = cur_group[-1][-1]
                            if this_concat_edge.to_node == last_concat_edge.to_node and this_concat_edge.to_idx == last_concat_edge.to_idx + 1:
                                cur_group.append(concat_node_edges)
                                continue
                            if len(cur_group) > 1:
                                edge_groups.append(cur_group)
                        cur_group = [concat_node_edges]
                        continue
                if cur_group:
                    if len(cur_group) > 1:
                        edge_groups.append(cur_group)
                    cur_group = None
            if cur_group:
                if len(cur_group) > 1:
                    edge_groups.append(cur_group)
                cur_group = None
        # we leave the splits and concats after this since they will be cleared up by remove_noops
        for edge_group in edge_groups:
            split_node = edge_group[0][0].from_node
            concat_node = edge_group[0][-1].to_node
            from_idx = edge_group[0][0].from_idx
            to_idx = edge_group[-1][0].from_idx
            from_concat_idx = edge_group[0][-1].to_idx
            to_concat_idx = edge_group[1][-1].to_idx
            LOG.info(
                f"combining outputs {from_idx}:{to_idx} on split node {split_node.name} followed by concat {concat_node.name}")
            # combine slices and shapes on edges in group
            new_slice, new_shape = reduce_slices(
                split_node.act_slices[from_idx:to_idx+1],
                split_node.out_shapes[from_idx:to_idx+1]
            )
            new_concat_shape = Dim.combine(
                [concat_node.in_dims[idx]
                    for idx in range(from_concat_idx, to_concat_idx+1)],
                concat_node.axis)
            split_node.act_slices = split_node.act_slices[:from_idx] + [
                new_slice] + split_node.act_slices[to_idx+1:]
            # the slice may need to reshape since we will remove everything in between
            split_node.out_shapes = split_node.out_shapes[:from_idx] + [
                new_concat_shape.shape] + split_node.out_shapes[to_idx+1:]

            # remove all edges and intermediate nodes on all edge groups
            for edge_list in edge_group:
                remove_edges(G, edge_list)
            # add back a direct edge to the first idx
            G.add_edge(NNEdge(from_node=split_node, from_idx=edge_group[0][0].from_idx, to_node=concat_node, to_idx=edge_group[0][-1].to_idx))
            out_edge_bundles = G.indexed_out_edges(split_node)
            # move edges beyond the edge group after the first index
            for offset, edge_list in enumerate(out_edge_bundles[to_idx+1:]):
                assert len(edge_list) == 1
                edge = edge_list[0]
                G.remove_edge(edge)
                G.add_edge(NNEdge.clone(edge, from_idx=from_idx+1+offset))
            # reindex the in edges in the concat
            from_idx = edge_group[0][-1].to_idx
            to_idx = edge_group[-1][-1].to_idx
            in_edges = G.indexed_in_edges(concat_node)
            for offset, in_edge in enumerate(in_edges[to_idx+1:]):
                G.remove_edge(in_edge)
                G.add_edge(NNEdge.clone(in_edge, to_idx=from_idx+1+offset))

        return bool(edge_groups)
