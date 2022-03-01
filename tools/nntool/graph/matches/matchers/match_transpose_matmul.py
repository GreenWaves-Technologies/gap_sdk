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

from graph.manipulations.eliminate_transposes.transpose_helpers import \
    identity_transpose
from graph.types import MatMulOpParameters, NNEdge, TransposeParameters
from graph.types.tensor_arithmetic import MatMulTransposedParameters
from utils.graph import GraphView

from ..matcher import (Matcher, description, groups, match_name, run_after,
                       run_before)

LOG = logging.getLogger("nntool." + __name__)

@run_after('fuse_external_bias_matmul')
@run_before('fuse_op_activation_scale8', 'fuse_op_activation_pow2')
@groups('*')
@match_name("match_transpose_matmul")
@description("spots Transpose followed by matmul and generates the proper matmul generator")
class MatchTransMatMul(Matcher):

    def get_node_list(self, G, params, result):
        if not result.add_node(G, params):
            return result
        out_edges = G.out_edges(params.name)
        if len(out_edges) > 1:
            return result
        return self.get_node_list(G, out_edges[0].to_node, result)

    def _match(self, G: GraphView, set_identity: bool = True, **kwargs):
        has_modified_graph = False

        for node in G.nodes(node_classes=MatMulOpParameters):
            in_edges = [edge for edge in G.indexed_in_edges(node.name)]
            trans_node = in_edges[1].from_node
            if not isinstance(trans_node, TransposeParameters):
                continue
            transpose = tuple(trans_node.transpose)
            if not identity_transpose(transpose[:-2]):
                continue
            if transpose[-2:] != (len(transpose) - 1, len(transpose) - 2):
                continue
            if isinstance(node, MatMulTransposedParameters):
                new_node = MatMulOpParameters(node.name)
            else:
                new_node = MatMulTransposedParameters(node.name)

            in_trans_edge = [edge for edge in G.indexed_in_edges(trans_node.name)][0]
            G.replace_node(node.name, new_node)
            G.remove(trans_node)
            G.add_edge(NNEdge(in_trans_edge.from_node, new_node, from_idx=in_trans_edge.from_idx, to_idx=1))
            has_modified_graph = True

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
