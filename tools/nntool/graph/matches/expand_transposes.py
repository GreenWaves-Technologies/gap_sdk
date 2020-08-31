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

from graph.matches.matcher import Matcher
from graph.types import NNEdge, Transposable, TransposeParameters
from utils.graph import GraphView


def apply_reverse_transpose_to_hint(hint, transpose):
    reverse_transpose = {transpose[i]: v for i, v in enumerate(hint)}
    reversed_hint = [reverse_transpose[idx] for idx in range(len(hint))]
    return reversed_hint


class ExpandTransposesMatcher(Matcher):
    NAME = "expand_transposes"
    DESCRIPTION = "Extract transposes from Transposable nodes for model generation"
    MODIFIES_DIMENSIONS = True
    def match(self, G: GraphView, set_identity: bool = True):
        # get a list of all the nodes that are transposable but not transposes
        # Need to do this first to avoid mutating it when doing the modifications
        tnodes = list(filter(lambda n: isinstance(n, Transposable) and
                             not isinstance(n, TransposeParameters),
                             G.nodes()))
        has_modified_graph = False
        for node in tnodes:
            if node.transpose_in:
                for idx, edge in enumerate(G.in_edges(node.name)):
                    if edge.to_idx >= len(node.transpose_in):
                        continue
                    trans = node.transpose_in[edge.to_idx]
                    if trans is None:
                        continue
                    has_modified_graph = True
                    in_params = TransposeParameters("%s_TIN_%s" % (node.name, idx),
                                                    transpose=trans)
                    if node.in_dims_hint and node.in_dims_hint[edge.to_idx]:
                        in_hint = node.in_dims_hint[edge.to_idx]
                        out_hint = apply_reverse_transpose_to_hint(in_hint, trans)
                        in_params.in_dims_hint = [in_hint.copy()]
                        in_params.out_dims_hint = [out_hint.copy()]
                        node.in_dims_hint[edge.to_idx] = out_hint
                    if G.quantization:
                        G.quantization.copy_to_node(node, in_params)
                    G.insert_node(in_params, edge.from_node.name, edge.to_node.name,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx,
                                  edge_class=NNEdge)
                node.transpose_in = None
            if node.transpose_out:
                for idx, edge in enumerate(G.out_edges(node.name)):
                    if edge.from_idx >= len(node.transpose_out):
                        continue
                    trans = node.transpose_out[edge.from_idx]
                    if trans is None:
                        continue
                    has_modified_graph = True
                    out_params = TransposeParameters("%s_TOUT_%s" % (node.name, idx),
                                                     transpose=trans)
                    if node.out_dims_hint:
                        out_hint = node.out_dims_hint[edge.from_idx]
                        in_hint = apply_reverse_transpose_to_hint(out_hint, trans)
                        out_params.in_dims_hint = [in_hint.copy()]
                        out_params.out_dims_hint = [out_hint.copy()]
                        node.out_dims_hint[edge.from_idx] = in_hint
                    if G.quantization:
                        G.quantization.copy_to_node(node, out_params)
                    G.insert_node(out_params, edge.from_node.name, edge.to_node.name,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx,
                                  edge_class=NNEdge)
                node.transpose_out = None
        if set_identity:
            self.set_identity(G)
        return has_modified_graph
