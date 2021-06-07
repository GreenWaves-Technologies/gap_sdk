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

from graph.dim import Dim
from graph.types import GlobalPoolParameters, NNEdge, ReshapeParameters
from utils.graph import GraphView
from utils.node_id import NodeId

from ..matcher import Matcher, groups, match_name, description

LOG = logging.getLogger("nntool." + __name__)


def reduce_reduction(state, params):
    if state is None:
        reduction_axes = set()
        in_shape = params.in_dims[0].shape
        axis_idxes = list(range(len(in_shape)))
        has_keepdims = False
    else:
        reduction_axes, in_shape, has_keepdims, axis_idxes = state

    reduced_axes = (axis_idxes[axis] for axis in params.axis)
    reduction_axes |= set(reduced_axes)
    if params.keep_dims:
        has_keepdims = True
        new_shape = tuple(
            1 if idx in params.axes else dim for idx, dim in enumerate(in_shape))
    else:
        new_shape = tuple(dim for idx, dim in enumerate(
            in_shape) if idx not in params.axis)
        axis_idxes = tuple(axis_idxes[idx] for idx, _ in enumerate(
            in_shape) if idx not in params.axis)

    return (reduction_axes, new_shape, has_keepdims, axis_idxes)

@groups('*')
@match_name("combine_reductions")
@description("""Combine reduction steps""")
class CombineReductions(Matcher):

    @staticmethod
    def reductions(G, node, first_node=None):
        if first_node is None:
            first_node = node
        if isinstance(node, GlobalPoolParameters) and node.pool_type == first_node.pool_type:
            out_edges = G.out_edges(node.name)
            if len(out_edges) > 1:
                return [node]
            return [node] + CombineReductions.reductions(G, out_edges[0].to_node, first_node=first_node)
        return []

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        nodes = list(G.nodes(node_classes=GlobalPoolParameters))
        modified_graph = False
        while nodes:
            node = nodes.pop()
            node_group = self.reductions(G, node)
            if len(node_group) <= 1:
                continue
            modified_graph = True
            reduction_axes, new_shape, has_keepdims, _ = reduce(
                reduce_reduction, node_group, None)
            new_node = node_group[0]
            new_node.axis = sorted(list(reduction_axes))
            new_node.keep_dims = has_keepdims
            out_edges = G.out_edges(node_group[-1].name)
            if G.quantization:
                last_qrec = G.quantization[NodeId(node_group[-1])]
                G.quantization[NodeId(new_node)].out_qs = last_qrec.out_qs
            for node in node_group[1::]:
                G.remove(node.name)
                nid = NodeId(node)
                if G.quantization and nid in G.quantization:
                    del G.quantization[nid]
            if has_keepdims and len(new_shape) != len(new_node.in_dims[0].shape):
                rparams = ReshapeParameters(
                    G.unique_name(f'{new_node.name}_reshape'),
                    shape=Dim.unnamed(new_shape))
                if G.quantization:
                    G.quantization.copy_qrec(last_qrec, 'out', 0, rparams)
                G.add_edge(NNEdge(new_node, rparams))
                new_node = rparams
            for edge in out_edges:
                G.add_edge(NNEdge(new_node, edge.to_node, to_idx=edge.to_idx))

        if set_identity:
            self.set_identity(G)

        return modified_graph
