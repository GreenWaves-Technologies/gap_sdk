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
import math

from graph.types import (ConstantInputParameters, HSigmoidActivationParameters,
                         MatrixAddParameters,
                         MatrixBroadcastedLinearOpParameters,
                         MatrixMulParameters, NNEdge, ReluActivationParameters)
from quantization.float32.float32_quantization import Float32QuantizationRecord
from quantization.multiplicative.mult_quantization import (
    MultQuantizationRecord, MultQuantizationRecordBase)
from quantization.multiplicative.symmetric.symmetric_mult_qtype_wrapper import \
    SymmetricMultQTypeWrapper
from quantization.symmetric.symmetric_quantization import \
    SymmetricQuantizationRecord
from utils.graph import Edge, GraphView
from utils.node_id import NodeId

from .matcher import DefaultMatcher, Matcher, MatchNode

LOG = logging.getLogger("nntool." + __name__)


def check_equals(G, node, val):
    if node.value is None or len(node.value) != 1:
        return False

    if G.has_quantized_parameters:
        qrec = G.quantization[NodeId(node)]
        if isinstance(qrec.out_qs[0], SymmetricMultQTypeWrapper):
            node_val = qrec.out_qs[0].wrapped.dequantize(node.value)
        else:
            node_val = qrec.out_qs[0].dequantize(node.value)
    else:
        node_val = node.value
    node_val = node_val.reshape((1,))[0]
    if val < 0:
        node_val = 1.0/node_val
        val = 1.0/val
    return math.floor(0.5 + node_val) == math.floor(0.5 + val)

# Matches filter -> mul with 1/6th constant


class MatchCloseHSigmoid(DefaultMatcher):
    NAME = 'match_close_hsigmoid'
    DESCRIPTION = 'Match relu6 followed by matmul with 1/6 constant and replaces with hsigmoid activation'

    def match_function(self, G: GraphView):
        sub = GraphView()
        sub.add_node(MatchNode('0', matcher=lambda node:
                               isinstance(node, ReluActivationParameters) and node.upper_bound == 6))
        sub.add_node(MatchNode('1', matcher=lambda node:
                               isinstance(node, MatrixMulParameters)))
        sub.add_node(MatchNode('2', matcher=lambda node:
                               isinstance(node, ConstantInputParameters) and check_equals(G, node, 1.0/6.0)))
        sub.add_edge(Edge('0', '1', to_idx=0))
        sub.add_edge(Edge('2', '1', to_idx=1))

        return G.match_fragment(sub)

    def replace_function(self, G: GraphView, subgraph: GraphView):
        relu_node = None
        constant_node = None
        mul_node = None
        for node in subgraph.nodes():
            if isinstance(node, ReluActivationParameters):
                relu_node = node
            elif isinstance(node, ConstantInputParameters):
                constant_node = node
            elif isinstance(node, MatrixMulParameters):
                mul_node = node

        activation = HSigmoidActivationParameters(mul_node.name + "_fused_close_hsigmoid", offset=0)

        if G.quantization:
            reluqrec = G.quantization[NodeId(relu_node)]
            mulqrec = G.quantization[NodeId(mul_node)]
            del G.quantization[NodeId(constant_node)]
            if isinstance(reluqrec, (SymmetricQuantizationRecord)):
                pqrec = SymmetricQuantizationRecord(
                    in_qs=reluqrec.in_qs, out_qs=mulqrec.out_qs)
            elif isinstance(reluqrec, (MultQuantizationRecord)):
                pqrec = MultQuantizationRecord(in_qs=reluqrec.in_qs, out_qs=mulqrec.out_qs)
            elif isinstance(reluqrec, (Float32QuantizationRecord)):
                pqrec = Float32QuantizationRecord(in_qs=reluqrec.in_qs, out_qs=mulqrec.out_qs)
            else:
                raise NotImplementedError()
            G.quantization[NodeId(activation)] = pqrec
        return activation, None, None


def look_back(G, node, state=None):
    # TODO - Pass through nodes that don't modify the tensor contents
    if state is None:
        state = {'relu1': None, 'add': None, 'relu2': None, 'mul': None, 'relu3': None}
    qrec = G.quantization.get(NodeId(node))
    if not isinstance(qrec, MultQuantizationRecordBase):
        return None
    if isinstance(node, ReluActivationParameters):
        if state['add']:
            state['relu1'] = None # (node, qrec)
        elif node.upper_bound == 6:
            state['relu2'] = (node, qrec)
        else:
            return None
        return look_back(G, G.in_edges(node.name)[0].from_node, state=state)
    elif isinstance(node, MatrixBroadcastedLinearOpParameters):
        edges = G.in_edges(node.name)
        if isinstance(edges[0].from_node, ConstantInputParameters):
            const_edge_idx = 0
            nonconst_edge = edges[1]
        elif isinstance(edges[1].from_node, ConstantInputParameters):
            const_edge_idx = 1
            nonconst_edge = edges[0]
        else:
            return None
        const_node = edges[const_edge_idx].from_node
        if len(const_node.value) != 1:
            return None
        if isinstance(node, MatrixMulParameters):
            if state['mul']:
                return None
            state['mul'] = (node, qrec, const_node)
        elif isinstance(node, MatrixAddParameters):
            if state['add'] or not check_equals(G, const_node, 3):
                return None
            state['add'] = (node, qrec, const_node)
        else:
            return None
        return look_back(G, nonconst_edge.from_node, state=state)
    else:
        if state['add'] and state['relu2'] and state['mul']:
            return state
    return None


def process_rec(G, oprec):
    mul_node = oprec['mul'][0]
    activation = HSigmoidActivationParameters(mul_node.name + "_fused_far_hsigmoid")
    G.add_node(activation)
    mulqrec = G.quantization[NodeId(mul_node)]
    G.quantization[NodeId(activation)] = mulqrec
    if oprec['relu1'] is not None:
        mulqrec.in_qs = oprec['relu1'][1].in_qs
        del G.quantization[NodeId(oprec['relu1'][0])]
        for edge in G.in_edges(oprec['relu1'][0].name):
            G.add_edge(NNEdge(from_node=edge.from_node, from_idx=edge.from_idx, to_node=activation.name))
        G.remove(oprec['relu1'][0])
    else:
        mulqrec.in_qs = oprec['add'][1].in_qs
        for edge in G.in_edges(oprec['add'][0].name):
            G.add_edge(NNEdge(from_node=edge.from_node, from_idx=edge.from_idx, to_node=activation.name))
    if oprec['relu3'] is not None:
        mulqrec.out_qs = oprec['relu3'][1].out_qs
        del G.quantization[NodeId(oprec['relu3'][0])]
        for edge in G.out_edges(oprec['relu3'][0].name):
            G.add_edge(NNEdge(to_node=edge.to_node, to_idx=edge.to_idx, from_node=activation.name))
        G.remove(oprec['relu3'][0])
    else:
        for edge in G.out_edges(oprec['mul'][0].name):
            G.add_edge(NNEdge(to_node=edge.to_node, to_idx=edge.to_idx, from_node=activation.name))

    del G.quantization[NodeId(oprec['relu2'][0])]
    G.remove(oprec['relu2'][0])
    for node_type in ('add', 'mul'):
        del G.quantization[NodeId(oprec[node_type][0])]
        G.remove(oprec[node_type][0])
        del G.quantization[NodeId(oprec[node_type][2])]
        G.remove(oprec[node_type][2])


class MatchFarHSigmoid(Matcher):
    NAME = 'match_far_hsigmoid'
    DESCRIPTION = 'Looks for quantized HSigmoid - [Relu] -> Add 3 -> Relu6 -> Mul 1/6 -> [Relu]'

    def match(self, G: GraphView, set_identity: bool = True):
        const_ops = [node for node in G.nodes()
                     if isinstance(node, MatrixMulParameters)
                     and any([isinstance(edge.from_node, ConstantInputParameters)
                              and check_equals(G, edge.from_node, 1.0/6.0)
                              for edge in G.in_edges(node.name)])]

        oprecs = [oprec for oprec in (look_back(G, op)
                                      for op in const_ops)
                  if oprec is not None]
        has_modified_graph = False
        for oprec in oprecs:
            mul_edge = G.out_edges(oprec['mul'][0].name)
            if len(mul_edge) == 1:
                mul_edge = mul_edge[0]
                if isinstance(mul_edge.to_node, ReluActivationParameters):
                    oprec['relu3'] = (mul_edge.to_node, G.quantization[NodeId(mul_edge.to_node)])
            has_modified_graph = True
            process_rec(G, oprec)

        if set_identity:
            self.set_identity(G)

        return has_modified_graph
