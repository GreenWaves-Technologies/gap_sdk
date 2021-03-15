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
from graph.types import GatherParameters, NNEdge, SplitParameters
from utils.graph import GraphView

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)

class GatherToSplitMatch(Matcher):
    NAME = "gather_to_split"
    DESCRIPTION = "collects gathers from a single node and converts to a split"

    def match(self, G: GraphView, set_identity: bool = True) -> bool:
        has_modified_graph = False
        gathers_by_origin = {}
        for gather in [node for node in G.nodes() if isinstance(node, GatherParameters)]:
            in_edge = G.in_edges(gather.name)[0]
            group = gathers_by_origin.setdefault((in_edge.from_node, in_edge.from_idx), [])
            group.append(gather)
        for in_edge, gathers in gathers_by_origin.items():
            # This is too difficult to handle if there are multiple slices
            axis = gathers[0].axis
            if not all(gather.axis == axis and len(gather.indices.shape) <= 1
                       for gather in gathers[1::]):
                continue
            # sort all the indices
            gathers = sorted(
                gathers,
                key=lambda x: x.indices if len(x.indices.shape) == 0 else x.indices[0])
            indices = [elem for gather in gathers
                       for elem in ([int(gather.indices)] if len(gather.indices.shape) == 0 else list(gather.indices))]
            # All the indices must be independant and sum to the out dim (this could be relaxed but
            # then needs to handle gaps)
            in_shape = in_edge[0].out_dims[in_edge[1]].shape
            in_shape_without_axis = in_shape[:axis:] + in_shape[axis+1::]
            if len(set(indices)) != len(indices) and len(set(indices)) == in_shape[axis]:
                continue
            # good for a split
            LOG.info("gathers from %s[%s] converted to a split", in_edge[0].name, in_edge[1])
            splits = []
            shapes = []
            out_edges = []
            for gather in gathers:
                splits.append([tuple([int(gather.indices), int(gather.indices) + 1, 1])])
                shapes.append(in_shape_without_axis)
                out_edges.append(G.out_edges(gather.name))
                G.remove(gather)
            params = SplitParameters("%s_split"%in_edge[0].name, act_slices=splits, out_shapes=shapes, axis=axis)
            if axis != 0:
                trans = [axis] + list(range(0, axis)) + list(range(axis, len(in_shape)))
                params.transpose_out = [[trans.index(idx) for idx in range(len(trans))]]
                params.transpose_in = [trans]
            for idx, edges in enumerate(out_edges):
                for edge in edges:
                    G.add_edge(NNEdge(from_node=params, to_node=edge.to_node, from_idx=idx, to_idx=edge.to_idx))
            G.add_edge(NNEdge(from_node=in_edge[0], to_node=params, from_idx=in_edge[1]))
            has_modified_graph = True

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
