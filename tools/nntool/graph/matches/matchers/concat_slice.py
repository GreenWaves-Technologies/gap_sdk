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
from functools import reduce

from graph.matches.match_utils import search_up
from graph.types import ConcatParameters, NNEdge
from graph.types.others import (NoOPParameters, ReshapeParameters,
                                StridedSliceParameters)
from utils.compatible_transposes import find_combination
from utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name,
                       needs_valid_dimension, run_qtune_on_match)

LOG = logging.getLogger("nntool." + __name__)


def info(msg):
    LOG.info(msg)

def validate_slice(node: StridedSliceParameters):
    if any(sl[2] != 1 for sl in node.act_slice):
        return [None] * 2
    slices_axes = node.slices_axes
    if len(slices_axes) != 1:
        return [None] * 2
    axis = slices_axes[0]
    act_slice = node.act_slice[axis]
    return axis, (act_slice[0], act_slice[1]-act_slice[0])

def up_from_slice(G, edge, axis, shape, remove_nodes=None, removing=True, reshape=None):
    if remove_nodes is None:
        remove_nodes = []
    if removing:
        remove_nodes.append(edge.to_node)
    node = edge.from_node
    if isinstance(node, ConcatParameters):
        if axis != node.axis:
            return [None] * 4
        offsets = node.offsets
        if shape[0] not in offsets:
            return [None] * 4
        index = offsets.index(shape[0])
        length = node.in_dims[index].shape[node.axis]
        if shape[1] != length:
            return [None] * 4
        return (remove_nodes, edge, index, reshape)
    else:
        if isinstance(node, ReshapeParameters):
            if reshape is None:
                reshape = node
            combinations = find_combination(node.shape, node.old_shape)
            combination = next(iter([comb for comb in combinations if (axis,) in comb]), None)
            if combination is None:
                return [None] * 4
            axis = combination.index((axis,))
        elif not isinstance(node, NoOPParameters):
            return [None] * 4
        return up_from_slice(
            G,
            G.in_edges(node)[0],
            axis,
            shape,
            remove_nodes=remove_nodes,
            removing=removing and len(G.out_edges(node)) == 1,
            reshape=reshape
        )

@groups('*')
@run_qtune_on_match
@needs_valid_dimension(True)
@match_name("concat_slice")
@description("removes slices after concats that match an input of the concat")
class ConcatSliceMatch(Matcher):

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs) -> bool:
        has_modified_graph = False
        concat_slice_edges = {}
        for slice_node in G.nodes(node_classes=StridedSliceParameters):
            # validate that slice is positive unit stride on a single axis
            axis, shape = validate_slice(slice_node)
            if axis is None:
                continue
            # search up for a concat
            remove_nodes, concat_edge, concat_in_idx, reshape = up_from_slice(
                G,
                G.in_edges(slice_node)[0],
                axis,
                shape)
            if not remove_nodes:
                continue
            concat = concat_edge.from_node
            concat_slice_edges.setdefault(
                concat, []).append((slice_node, remove_nodes, concat_in_idx, reshape))
        if not concat_slice_edges:
            return False

        for concat, slices in concat_slice_edges.items():
            for (slice_node, remove_nodes, concat_in_idx, reshape) in slices:
                self.eliminate_slice(G, concat, slice_node, remove_nodes, concat_in_idx, reshape)
            if not G.out_edges(concat):
                has_modified_graph = True
                concat_in_edges = G.in_edges(concat)
                info(f"removing concat {concat.name}")
                if concat_in_edges:
                    self.resolve_unused_concat_in_edges(
                        G, concat, concat_in_edges)
                if concat.name in G:
                    G.remove(concat)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph

    def search_delete_nodes_up(self, G, edge):
        out_edges = G.out_edges(edge.from_node)
        if len(out_edges) > 1:
            return []
        return [edge.from_node] + self.search_delete_nodes_up(G, G.in_edges(edge.from_node)[0])

    @staticmethod
    def edge_list_str(G, edges):
        edge_list = list(reversed([edge.to_node.name for edge in edges]))
        if len(G.out_edges(edges[-1].from_node)) == 1:
            edge_list.insert(0, f'{edges[-1].from_node.name} (removed)')
        else:
            edge_list.insert(0, edges[-1].from_node.name)
        return "->".join(edge_list)

    def resolve_unused_concat_in_edges(self, G, concat, concat_in_edges):
        for edge in concat_in_edges:
            nodes = self.search_delete_nodes_up(G, edge)
            if nodes:
                info(f"removing unused nodes {' '.join(node.name for node in nodes)}")
                for node in nodes:
                    if node.name in G:
                        G.remove(node)

    def eliminate_slice(self, G, concat, slice_node, remove_nodes, concat_in_idx, reshape_in):
        concat_in_edge = G.indexed_in_edges(concat)[concat_in_idx]
        node_idx = (concat_in_edge.from_node, concat_in_edge.from_idx)
        info(f'removing slice {slice_node.name} and connecting {concat_in_edge.from_node.name}:{concat_in_edge.from_idx} to its edges')
        if reshape_in:
            reshape = ReshapeParameters(
                G.unique_name(f'{slice_node.name}_reshape'),
                old_shape=concat_in_edge.from_node.out_dims[concat_in_edge.from_idx].shape,
                shape=slice_node.out_shape)
        elif slice_node.changes_shape:
            reshape = ReshapeParameters(
                G.unique_name(f'{slice_node.name}_reshape'),
                old_shape=slice_node.post_slice_shape,
                shape=slice_node.out_shape)
        else:
            reshape = None
        if reshape:
            G.add_edge(
                NNEdge(
                    from_node=node_idx[0],
                    from_idx=node_idx[1],
                    to_node=reshape))
            node_idx = (reshape, 0)

        slice_out_edges = G.out_edges(slice_node)
        for rnode in remove_nodes:
            if rnode.name in G:
                G.remove(rnode)
        if slice_node.name in G:
            G.remove(slice_node)
        for edge in slice_out_edges:
            G.add_edge(NNEdge(from_node=node_idx[0], from_idx=node_idx[1],
                              to_node=edge.to_node, to_idx=edge.to_idx))
