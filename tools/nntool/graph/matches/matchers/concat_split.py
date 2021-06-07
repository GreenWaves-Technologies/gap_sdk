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

from graph.types import ConcatParameters, NNEdge, SplitParameters
from utils.graph import GraphView

from ..matcher import Matcher, groups, match_name, description

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
            if concat_node.transpose_out or split_node.transpose_in:
                continue
            if concat_node.axis != split_node.axis:
                continue
            axis = concat_node.axis
            split_out_sizes = [out_shape[axis] for out_shape in split_node.out_shapes]
            if len(split_out_sizes) != len(concat_node.in_dims):
                continue
            if not all(split_out_sizes[idx] == in_dim.shape[axis] for idx, in_dim in enumerate(concat_node.in_dims)):
                continue
            has_modified_graph = True
            LOG.info("removing unnecessary concat/split pair %s/%s", concat_node.name, split_node.name)
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
