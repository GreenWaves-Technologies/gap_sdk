# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from utils.graph import GraphView

from ..types.base import Transposable
from ..types.others import TransposeParameters

from .matcher import Matcher


def apply_reverse_transpose_to_hint(hint, transpose):
    reverse_transpose = {transpose[i] : v for i, v in enumerate(hint)}
    reversed_hint = [reverse_transpose[idx] for idx in range(len(hint))]
    return reversed_hint

class ExpandTransposesMatcher(Matcher):
    NAME = "expand_transposes"
    DESCRIPTION = "Extract transposes from Transposable nodes for model generation"

    def match(self, G: GraphView, set_identity: bool = True):
        # get a list of all the nodes that are transposable but not transposes
        # Need to do this first to avoid mutating it when doing the modifications
        tnodes = list(filter(lambda n: isinstance(n, Transposable) and\
                                not isinstance(n, TransposeParameters),
                             G.nodes()))
        for node in tnodes:
            if node.transpose_in:
                for idx, edge in enumerate(G.in_edges(node.name)):
                    in_params = TransposeParameters("%s_TIN_%s" % (node.name, idx),
                                                    transpose=node.transpose_in)
                    if node.in_dims_hint:
                        in_hint = node.in_dims_hint[edge.to_idx]
                        out_hint = apply_reverse_transpose_to_hint(in_hint, node.transpose_in)
                        in_params.in_dims_hint = [in_hint.copy()]
                        in_params.out_dims_hint = [out_hint.copy()]
                        node.in_dims_hint[edge.to_idx] = out_hint
                    G.insert_node(in_params, edge.from_node.name, edge.to_node.name,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx)
                node.transpose_in = None
            if node.transpose_out:
                for idx, edge in enumerate(G.out_edges(node.name)):
                    out_params = TransposeParameters("%s_TOUT_%s" % (node.name, idx),
                                                     transpose=node.transpose_out)
                    if node.out_dims_hint:
                        out_hint = node.out_dims_hint[edge.from_idx]
                        in_hint = apply_reverse_transpose_to_hint(out_hint, node.transpose_out)
                        out_params.in_dims_hint = [in_hint.copy()]
                        out_params.out_dims_hint = [out_hint.copy()]
                        node.out_dims_hint[edge.from_idx] = in_hint
                    G.insert_node(out_params, edge.from_node.name, edge.to_node.name,
                                  from_idx=edge.from_idx, to_idx=edge.to_idx)
                node.transpose_out = None
        if set_identity:
            self.set_identity(G)
