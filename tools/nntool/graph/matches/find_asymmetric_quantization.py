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

from graph.matches.matcher import Matcher
from graph.types import (ActivationParameters, ConcatParameters,
                         ConstantInputParameters, Conv2DParameters,
                         ConvFusionParameters, FcParameters,
                         GlobalPoolParameters, InputParameters,
                         MatrixAddParameters, OutputParameters,
                         PoolingParameters, ReshapeParameters)
from quantization.multiplicative.symmetric.symmetric_mult_qtype_wrapper import \
    SymmetricMultQTypeWrapper
from utils.graph import GraphView
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)

CAN_CHANGE_OUTPUT = (
    InputParameters, ConstantInputParameters, Conv2DParameters,
    ConvFusionParameters, FcParameters, MatrixAddParameters
)

CAN_CHANGE_INPUT = (
    OutputParameters, Conv2DParameters, ConvFusionParameters,
    FcParameters, MatrixAddParameters
)

CAN_PROPAGATE_INPUT = (
    GlobalPoolParameters, ReshapeParameters, ConcatParameters, ActivationParameters, PoolingParameters
)

ARE_MULTI_INPUT = (
    ConcatParameters
)

class FindAsymmetricQuantization(Matcher):
    NAME = "find_asymmetric_quantization"
    DESCRIPTION = """Find nodes that can have asymmetric quantization. Must run after padding has been fused."""

    def can_change_input(self, G, node, exclude=None):
        """Returns None or a list of tuples of (node, multi_input_node) where node is an
        input of multi_input_node. An empty list is a confirmed string. A list that contains
        multi input nodes needs to be reconciled. An empty list means that this node
        cannot be changed."""

        if isinstance(node, CAN_PROPAGATE_INPUT):
            if exclude and node in exclude:
                return None
            nodes = []
            for succ in [succ
                         for succs in G.successors(node.name)
                         for succ in succs]:
                can_change = self.can_change_input(G, succ, exclude=exclude)
                if can_change is None:
                    return None
                nodes += can_change
                if isinstance(succ, ARE_MULTI_INPUT):
                    nodes.append((node, succ))
            return nodes
        if not isinstance(node, CAN_CHANGE_INPUT):
            return None
        if isinstance(node, ConvFusionParameters):
            filters = node.contained_filters()
            if len(filters) == 1 and not filters[0].padding.has_padding:
                return []
            else:
                return None
        if isinstance(node, Conv2DParameters):
            return None if node.padding.has_padding else []
        return []

    def can_change_output(self, node):
        return isinstance(node, CAN_CHANGE_OUTPUT)

    def validate_multi_input(self, G, input_dict):
        # {start_node: [(pred, mi_node), ..]}
        mi_nodes = {}
        # index all of the predecessor nodes by mi node
        for pr_node, mi_node in [match for matches in input_dict.values() for match in matches]:
            pr_node_set = mi_nodes.get(mi_node)
            if pr_node_set is None:
                pr_node_set = set()
                mi_nodes[mi_node] = pr_node_set
            pr_node_set.add(pr_node)
        bad_mi_nodes = []
        # check that all the predecessors were OK
        for mi_node, pr_nodes in mi_nodes.items():
            if not all(node in pr_nodes for node in G.predecessors(mi_node)):
                bad_mi_nodes.append(mi_node)
        start_nodes = []
        # find the records that have bad nodes in them
        if bad_mi_nodes:
            for start_node, matches in input_dict.items():
                if any(mi_node in bad_mi_nodes for _, mi_node in matches):
                    start_nodes.append(start_nodes)
        for start_node in start_nodes:
            del input_dict[start_node]
            matches = self.can_change_input(G, start_node, exclude=bad_mi_nodes)
            if matches is not None:
                assert len(matches) == 0
                input_dict[start_node] = []
        return input_dict

    def change_output_to_async(self, G, node, idx):
        if isinstance(node, ConvFusionParameters):
            changing = False
            for fnode in node.contained_nodes():
                if changing:
                    nid = NodeId(node, fnode)
                    qrec = G.quantization[nid]
                    if isinstance(qrec.in_qs[0], SymmetricMultQTypeWrapper):
                        qrec.in_qs[0] = qrec.in_qs[0].wrapped
                    if isinstance(qrec.out_qs[0], SymmetricMultQTypeWrapper):
                        qrec.out_qs[0] = qrec.out_qs[0].wrapped
                elif isinstance(fnode, (Conv2DParameters, FcParameters)):
                    changing = True
                    nid = NodeId(node, fnode)
                    qrec = G.quantization[nid]
                    if isinstance(qrec.out_qs[0], SymmetricMultQTypeWrapper):
                        qrec.out_qs[0] = qrec.out_qs[0].wrapped

        nid = NodeId(node)
        qrec = G.quantization[nid]
        if isinstance(qrec.out_qs[idx], SymmetricMultQTypeWrapper):
            qrec.out_qs[idx] = qrec.out_qs[idx].wrapped

    def change_input_to_async(self, G, node, idx):
        if isinstance(node, ConvFusionParameters):
            for fnode in node.contained_nodes():
                nid = NodeId(node, fnode)
                qrec = G.quantization[nid]
                if isinstance(fnode, (Conv2DParameters, FcParameters)):
                    if isinstance(qrec.in_qs[0], SymmetricMultQTypeWrapper):
                        qrec.in_qs[0] = qrec.in_qs[0].wrapped
                        qrec.biases_q.link(qrec.weights_q, qrec.in_qs[0])
                    return
                if isinstance(qrec.in_qs[0], SymmetricMultQTypeWrapper):
                    qrec.in_qs[0] = qrec.in_qs[0].wrapped
                if isinstance(qrec.out_qs[0], SymmetricMultQTypeWrapper):
                    qrec.out_qs[0] = qrec.out_qs[0].wrapped

        nid = NodeId(node)
        qrec = G.quantization[nid]
        if isinstance(qrec.in_qs[idx], SymmetricMultQTypeWrapper):
            qrec.in_qs[idx] = qrec.in_qs[idx].wrapped
            if isinstance(node, (Conv2DParameters, FcParameters)):
                qrec.biases_q.link(qrec.weights_q, qrec.in_qs[idx])
        if isinstance(node, OutputParameters) and isinstance(qrec.out_qs[0], SymmetricMultQTypeWrapper):
            qrec.out_qs[0] = qrec.out_qs[0].wrapped

    def do_change(self, G, node, idx=0):
        self.change_output_to_async(G, node, idx)
        for edge in G.out_edges(node.name):
            if isinstance(edge.to_node, CAN_PROPAGATE_INPUT):
                self.change_input_to_async(G, edge.to_node, edge.to_idx)
                self.do_change(G, edge.to_node, edge.from_idx)
            else:
                assert isinstance(edge.to_node, CAN_CHANGE_INPUT)
                if isinstance(edge.to_node, ConvFusionParameters):
                    filters = edge.to_node.contained_filters()
                    assert len(filters) == 1 and not filters[0].padding.has_padding
                if isinstance(edge.to_node, Conv2DParameters):
                    assert not edge.to_node.padding.has_padding
                self.change_input_to_async(G, edge.to_node, edge.to_idx)

    def match(self, G: GraphView, set_identity: bool = True):
        if not G.quantization:
            return
        input_dict = {}
        for node in G.nodes():
            if not self.can_change_output(node):
                continue
            all_matches = []
            for succ in [succ for succs in G.successors(node.name) for succ in succs]:
                matches = self.can_change_input(G, succ)
                if matches is None:
                    all_matches = None
                    break
                all_matches += matches
            if all_matches is None:
                continue
            input_dict[node] = all_matches

        input_dict = self.validate_multi_input(G, input_dict)
        for node in input_dict:
            # all nodes that can currently change output have one output
            self.do_change(G, node)

        if set_identity:
            self.set_identity(G)
        return False
