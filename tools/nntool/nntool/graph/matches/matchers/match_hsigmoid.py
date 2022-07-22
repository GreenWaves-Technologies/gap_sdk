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

# import math
import numpy as np
from nntool.graph.types import (ConstantInputNode, HSigmoidNode,
                         MatrixAddNode,
                         MatrixBroadcastedLinearOpNodeBase,
                         MatrixMulNode, NNEdge, ReluNode)
from nntool.quantization.new_qrec import QRec
from nntool.utils.graph import GraphView

from ..matcher import Matcher, description, groups, match_name, run_before

LOG = logging.getLogger(__name__)

THRESHOLD = 1 / 127


def check_equals(G, node, val):
    if node.value is None or node.value.size != 1:
        return False

    node_val = node.dqvalue.flatten()

    if val < 0:
        node_val = 1.0/node_val
        val = 1.0/val
    return np.all(np.abs(node_val - val) < THRESHOLD)

# Matches filter -> mul with 1/6th constant


@match_name('match_close_hsigmoid')
@description('Match relu6 followed by matmul with 1/6 constant and replaces with hsigmoid activation')
@groups('scaled')
@run_before('fuse_gap_convs', 'fuse_gap_linear')
class MatchCloseHSigmoid(Matcher):

    def _match(self, G: GraphView, **kwargs):
        something_changed = False
        for relu_node in [node for node in G.nodes(node_classes=ReluNode) if node.upper_bound == 6]:
            out_edges = G.out_edges(relu_node)
            if len(out_edges) != 1 or not isinstance(out_edges[0].to_node, MatrixMulNode):
                continue
            mul_node = out_edges[0].to_node
            in_edges = G.in_edges(mul_node)
            if len(in_edges) != 2:
                continue
            other_edge = (set(in_edges) - {out_edges[0]}).pop()
            constant_node = other_edge.from_node
            if len(G.out_edges(constant_node)) != 1:
                continue
            if (not isinstance(constant_node, ConstantInputNode) or
                    not check_equals(G, constant_node, 1.0/6.0)):
                continue

            something_changed = True
            activation = HSigmoidNode(
                G.unique_name(f'{mul_node.name}_hsigmoid'), offset=0)

            in_edges = G.in_edges(relu_node)
            out_edges = G.out_edges(mul_node)

            nodes_to_replace = [relu_node, mul_node, constant_node]

            LOG.info(f'fusing {", ".join(node.name for node in nodes_to_replace)} into HSIGMOID {activation.name}')
            G.remove_all(nodes_to_replace)

            for in_edge in in_edges:
                G.add_edge(NNEdge.clone(in_edge, to_node=activation, to_idx=0))
            for out_edge in out_edges:
                G.add_edge(NNEdge.clone(
                    out_edge, from_node=activation, from_idx=0))

            if G.quantization:
                reluqrec = G.quantization[relu_node.name]
                mulqrec = G.quantization[mul_node.name]
                del G.quantization[constant_node.name]
                pqrec = QRec.copy_ktype(
                    reluqrec, in_qs=reluqrec.in_qs, out_qs=mulqrec.out_qs)
                G.quantization[activation.name] = pqrec

        return something_changed


def look_back(G, node, state=None):
    # TODO - Pass through nodes that don't modify the tensor contents
    if G.quantization is None:
        return None
    if state is None:
        state = {'relu1': None, 'add': None,
                 'relu2': None, 'mul': None, 'relu3': None}
    qrec = G.quantization.get(node.name)
    if not qrec or qrec.ktype != 'scaled':
        return None
    if isinstance(node, ReluNode):
        if state['add']:
            state['relu1'] = None  # (node, qrec)
        elif node.upper_bound == 6:
            state['relu2'] = (node, qrec)
        else:
            return None
        return look_back(G, G.in_edges(node.name)[0].from_node, state=state)
    elif isinstance(node, MatrixBroadcastedLinearOpNodeBase):
        edges = G.in_edges(node.name)
        if isinstance(edges[0].from_node, ConstantInputNode):
            const_edge_idx = 0
            nonconst_edge = edges[1]
        elif isinstance(edges[1].from_node, ConstantInputNode):
            const_edge_idx = 1
            nonconst_edge = edges[0]
        else:
            return None
        const_node = edges[const_edge_idx].from_node
        if const_node.value.size != 1:
            return None
        if isinstance(node, MatrixMulNode):
            if state['mul']:
                return None
            state['mul'] = (node, qrec, const_node)
        elif isinstance(node, MatrixAddNode):
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
    activation = HSigmoidNode(
        mul_node.name + "_fused_far_hsigmoid")
    G.add_node(activation)
    mulqrec = G.quantization[mul_node.name]
    G.quantization[activation.name] = mulqrec
    if oprec['relu1'] is not None:
        mulqrec.in_qs = oprec['relu1'][1].in_qs
        del G.quantization[oprec['relu1'][0].name]
        for edge in G.in_edges(oprec['relu1'][0].name):
            G.add_edge(NNEdge(from_node=edge.from_node,
                              from_idx=edge.from_idx, to_node=activation.name))
        G.remove(oprec['relu1'][0])
    else:
        mulqrec.in_qs = oprec['add'][1].in_qs
        for edge in G.in_edges(oprec['add'][0].name):
            G.add_edge(NNEdge(from_node=edge.from_node,
                              from_idx=edge.from_idx, to_node=activation.name))
    if oprec['relu3'] is not None:
        mulqrec.out_qs = oprec['relu3'][1].out_qs
        del G.quantization[oprec['relu3'][0].name]
        for edge in G.out_edges(oprec['relu3'][0].name):
            G.add_edge(NNEdge(to_node=edge.to_node,
                              to_idx=edge.to_idx, from_node=activation.name))
        G.remove(oprec['relu3'][0])
    else:
        for edge in G.out_edges(oprec['mul'][0].name):
            G.add_edge(NNEdge(to_node=edge.to_node,
                              to_idx=edge.to_idx, from_node=activation.name))

    del G.quantization[oprec['relu2'][0].name]
    G.remove(oprec['relu2'][0])
    for node_type in ('add', 'mul'):
        del G.quantization[oprec[node_type][0].name]
        G.remove(oprec[node_type][0])
        del G.quantization[oprec[node_type][2].name]
        G.remove(oprec[node_type][2])


@match_name('match_far_hsigmoid')
@description('Looks for quantized HSigmoid - [Relu] -> Add 3 -> Relu6 -> Mul 1/6 -> [Relu]')
@groups('scaled')
@run_before('match_close_hsigmoid')
class MatchFarHSigmoid(Matcher):

    def _match(self, G: GraphView, **kwargs):
        const_ops = [node for node in G.nodes()
                     if isinstance(node, MatrixMulNode)
                     and any([isinstance(edge.from_node, ConstantInputNode)
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
                if isinstance(mul_edge.to_node, ReluNode):
                    oprec['relu3'] = (mul_edge.to_node,
                                      G.quantization[mul_edge.to_node.name])
            has_modified_graph = True
            process_rec(G, oprec)



        return has_modified_graph
