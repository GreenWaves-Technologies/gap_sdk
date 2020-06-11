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
from collections import OrderedDict

import numpy as np

from graph.nngraph import NNGraph
from graph.types import (ConstantInputParameters, Conv2DParameters,
                         ConvFusionParameters, FcParameters, InputParameters,
                         MatrixAddParameters, ActivationFusion,
                         MatrixBroadcastedLinearOpParameters,
                         MatrixSubParameters, MatScaleFusionParameters,
                         PoolingParameters, SoftMaxParameters, GlobalPoolParameters,
                         OutputParameters)
from quantization.multiplicative.mult_quantization import (
    MultAddQuantizationRecord, MultConstantQuantizationRecord,
    MultQuantizationRecord, MultScalableFilterQuantizationRecord)
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import \
    SymmetricMultBiasesQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import MultMulBiasScaleQType
from quantization.quantization_set import QuantizationSet
from quantization.quantizer import Quantizer
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId, convert_keys_to_str, convert_str_to_keys

LOG = logging.getLogger('nntool.' + __name__)

WIDTH_TO_DTYPE = {
    8: np.int8,
    16: np.int16
}


class MultQuantizer(Quantizer, JsonSerializable):
    def __init__(self, activation_stats, force_width=None, quantized_dimension=None, narrow_weights=False):
        self._activation_stats = activation_stats
        self._force_width = force_width
        self._quantized_dimension = quantized_dimension
        self._narrow_weights = narrow_weights

    # for tests
    def __eq__(self, value):
        return self._activation_stats == value._activation_stats and \
            self._force_width == value._force_width

    def _encapsulate(self):
        return {
            'activation_stats': convert_keys_to_str(self._activation_stats),
            'force_width': self._force_width,
            'quantized_dimension': self._quantized_dimension
        }

    @classmethod
    def _dencapsulate(cls, val):
        return MultQuantizer(convert_str_to_keys(val['activation_stats']),
                             val['force_width'],
                             val['quantized_dimension'])

    @staticmethod
    def get_in_qs(G, edge_recs, node):
        if isinstance(node, InputParameters):
            in_qs = []
        else:
            in_qs = [edge_recs[edge.params]
                     for edge in G.indexed_in_edges(node.name)]
        return in_qs

    def get_quantized_dimension(self, node):
        if self._quantized_dimension == 'tensor':
            return None
        elif self._quantized_dimension == 'channel':
            return node.filter.get_order_idx('out_c')
        return None

    def calculate_q(self, G, node, astats, in_qs, dtype, out_dtype=None):
        del G
        if out_dtype is None:
            out_dtype = dtype
        if isinstance(node, (PoolingParameters, OutputParameters)):
            o_q = in_qs[0]
        elif isinstance(node, SoftMaxParameters):
            o_q = SymmetricMultQType(min_val=-1, max_val=1, dtype=np.int16, scale=2**(-15))
        else:
            o_q = SymmetricMultQType.from_min_max(min_val=astats['min'],
                                                  max_val=astats['max'],
                                                  dtype=out_dtype)

        if isinstance(node, (MatrixAddParameters, MatrixSubParameters)):
            qrec = MultAddQuantizationRecord(in_qs=in_qs, out_qs=[o_q])

        elif isinstance(node, (MatrixBroadcastedLinearOpParameters, MatScaleFusionParameters, GlobalPoolParameters)):
            qrec = MultQuantizationRecord(in_qs=in_qs, out_qs=[o_q])

        elif isinstance(node, ConstantInputParameters):
            qrec = MultConstantQuantizationRecord(out_qs=[o_q],
                                                  constants_are_quantized=False)

        elif isinstance(node, (FcParameters, Conv2DParameters)):
            weights_q = SymmetricMultQType.from_array(arr=node.weights,
                                                      quantized_dimension=self.get_quantized_dimension(node),
                                                      dtype=dtype, narrow_range=self._narrow_weights)
            if node.has_bias:
                biases_q = SymmetricMultBiasesQType(dtype=np.int32, scale=weights_q.scale * in_qs[0].scale)
            else:
                biases_q = SymmetricMultBiasesQType(dtype=np.int32, scale=np.array([1], dtype=np.int32))
            mul_biases_q = MultMulBiasScaleQType.from_filter(in_qs[0], weights_q, o_q, node)
            qrec = MultScalableFilterQuantizationRecord(in_qs=[in_qs[0]],
                                                        out_qs=[o_q],
                                                        weights_q=weights_q,
                                                        biases_q=biases_q,
                                                        mul_biases_q=mul_biases_q,
                                                        constants_are_quantized=False)
            LOG.debug("filter %s qrec %s", node.name, qrec)
        else:
            qrec = MultQuantizationRecord(in_qs=in_qs, out_qs=[o_q])
        return qrec

    def quantize_fusion(self, G, node, in_qs, dtype):
        result = OrderedDict()
        fin_qs = in_qs
        nodes = node.contained_nodes()
        if node.fusion_type in ['conv_active_pool', 'conv_active']:
            conv_node = nodes[0]
            act_node = nodes[1]
            act_astats = self._activation_stats.get(NodeId(node, act_node))
            conv_qrec = self.calculate_q(G,
                                         conv_node,
                                         act_astats,
                                         fin_qs,
                                         dtype,
                                         out_dtype=np.int8)
            result[NodeId(node, conv_node)] = conv_qrec
            fin_qs = conv_qrec.out_qs
            nodes = nodes[1:]
        for fnode in nodes:
            qrec = self.calculate_q(G,
                                    fnode,
                                    self._activation_stats.get(NodeId(node, fnode)),
                                    fin_qs,
                                    dtype)
            result[NodeId(node, fnode)] = qrec
            fin_qs = qrec.out_qs
        return MultQuantizationRecord(in_qs=in_qs, out_qs=fin_qs), result

    def quantize_forward(self, G: NNGraph, edge_recs, dtype=np.int8, result=None):
        if result is None:
            result = QuantizationSet()
        for node in [step['node'] for step in G.graph_state.steps]:
            LOG.debug("quantize forward %s", node.name)
            in_qs = self.get_in_qs(G, edge_recs, node)
            if isinstance(node, (ConvFusionParameters, ActivationFusion)):
                qrec, qrecs = self.quantize_fusion(G, node, in_qs, dtype)
                for node_id, fqrec in qrecs.items():
                    result[node_id] = fqrec
            else:
                qrec = self.calculate_q(G,
                                        node,
                                        self._activation_stats.get(
                                            NodeId(node, None)),
                                        in_qs,
                                        dtype)
            result[NodeId(node, None)] = qrec
            if not qrec:
                break

            for edges in G.indexed_out_edges(node.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]
        return result

    def dequantize(self, G: NNGraph):
        qrecs = G.quantization
        LOG.info("dequantizing graph parameters")
        for _, node, _, fnode in G.nodes_iterator():
            qrec = qrecs[NodeId(node, fnode)]
            if isinstance(node, ConstantInputParameters):
                node.value = qrec.out_q[0].dequantize(node.value)
            else:
                anode = node if fnode is None else fnode
                if isinstance(anode, (FcParameters, Conv2DParameters)):
                    if anode.has_bias:
                        anode.biases = qrec.biases_q.dequantize(anode.biases)
                    anode.weights = qrec.weights_q.dequantize(anode.weights)

    def quantize(self, G: NNGraph) -> OrderedDict:
        '''quantize the graph'''
        if G.has_quantized_parameters:
            self.dequantize(G)
            G.has_quantized_parameters = False
            G.quantization = None
        edge_recs = {}
        dtype = WIDTH_TO_DTYPE[self._force_width]
        qrecs = self.quantize_forward(G, edge_recs, dtype)
        qrecs['__quantizer'] = self
        G.graph_identity.quantization_type = 'SQ8'
        return qrecs
