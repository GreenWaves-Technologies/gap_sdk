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
from graph.types import StridedSliceParameters, NNEdge, SplitParameters
from utils.graph import GraphView

from .matcher import Matcher

LOG = logging.getLogger("nntool." + __name__)


class SliceToSplitMatch(Matcher):
    NAME = "slice_to_split"
    DESCRIPTION = "collects slices from a single node and converts to a single split"

    def match(self, G: GraphView, set_identity: bool = True) -> bool:
        has_modified_graph = False
        slices_by_origin = {}
        for slice_node in [node for node in G.nodes() if isinstance(node, StridedSliceParameters)]:
            in_edge = G.in_edges(slice_node.name)[0]
            group = slices_by_origin.setdefault((in_edge.from_node, in_edge.from_idx), [])
            group.append(slice_node)
        for in_edge, slice_nodes in slices_by_origin.items():
            slices = list(zip(*[node.act_slice for node in slice_nodes]))
            diff_slices = [(idx, elems) for idx, elems in enumerate(slices)
                           if not all(elems[0] == elem for elem in elems[1::])]
            if len(diff_slices) != 1:
                continue
            # strides must be one
            if any(sl[2] != 1 for sl in diff_slices[0][1]):
                continue
            # check if slices are consecutive and non overlapping
            slices = sorted(diff_slices[0][1], key=lambda x: x[0])
            if not all(sl[0] + sl[1] == slices[i+1][0] for i, sl in enumerate(slices[:-1:])):
                continue
            szes = [sl[1] - sl[0] for sl in slices]
            axis = diff_slices[0][0]
            slice_nodes = sorted(slice_nodes, key=lambda x: x.act_slice[axis][0])
            act_slices, out_shapes, axis = SplitParameters.get_splits(slice_nodes[0].in_dims[0].shape, axis, splits=szes)
            params = SplitParameters(slice_nodes[0].name + '_split', act_slices=act_slices, out_shapes=out_shapes, axis=axis)
            in_edge = G.in_edges(slice_nodes[0].name)[0]
            G.add_edge(NNEdge(from_node=in_edge.from_node, to_node=params, from_idx=in_edge.from_idx))
            sub_names = []
            for idx, node in enumerate(slice_nodes):
                sub_names.append(node.name)
                out_edges = G.out_edges(node.name)
                G.remove(node)
                for out_edge in out_edges:
                    G.add_edge(NNEdge(from_node=params, to_node=out_edge.to_node, from_idx=idx, to_idx=out_edge.to_idx))
            LOG.info(f'replaced slice nodes {",".join(sub_names)} with split node {sub_names[0]}')

            has_modified_graph = True

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
