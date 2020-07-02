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
from math import pow

import numpy as np

from graph.nngraph import NNGraph
from graph.types import (ActivationFusion, ConstantInputParameters,
                         Conv2DParameters, ConvFusionParameters, FcParameters,
                         GlobalPoolParameters, InputParameters, LSTMParameters,
                         MatrixAddParameters, SplitParameters,
                         MatrixBroadcastedLinearOpParameters,
                         MatrixSubParameters, MatScaleFusionParameters,
                         OutputParameters, PoolingParameters, RNNParameters,
                         SoftMaxParameters)
from quantization.multiplicative.mult_quantization import (
    MultAddQuantizationRecord, MultConstantQuantizationRecord,
    MultQuantizationRecord, MultScalableFilterQuantizationRecord,
    MultScalableLstmQuantizationRecord, MultScalableRnnQuantizationRecord)
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import \
    MultMulBiasScaleQType
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import \
    SymmetricMultBiasesQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.quantization_set import QuantizationSet
from quantization.quantizer import Quantizer
from quantization.qtype import QType
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId, convert_keys_to_str, convert_str_to_keys
from utils.stats_funcs import calc_bits

LOG = logging.getLogger('nntool.' + __name__)

WIDTH_TO_DTYPE = {
    8: np.int8,
    16: np.int16
}

QUANTIZATION_OPTIONS = {
    RNNParameters:
        [
            {
                'name': 'mode',
                'type': str,
                'choices': ['autotiler', 'extended'],
                'help': 'scales state and input separately',
                'default': 'autotiler'
            },
            {
                'name': 'state_width',
                'type': int,
                'choices': ['8', '16'],
                'help': 'sets width of recurrent state',
                'default': 8
            },
            {
                'name': 'by_state',
                'type': bool,
                'help': 'sets width of recurrent state',
                'default': False
            }
        ]
}


class MultQuantizer(Quantizer, JsonSerializable):
    def __init__(self, activation_stats, force_width=None,
                 quantized_dimension=None, narrow_weights=False,
                 options=None):
        self._options = options or {}
        self._activation_stats = activation_stats
        self._force_width = force_width
        self._quantized_dimension = quantized_dimension
        self._narrow_weights = narrow_weights
        self.qrecs = None

    def get_options(self, node):
        if node.__class__ in QUANTIZATION_OPTIONS:
            options = {option['name']: option['default']
                       for option in QUANTIZATION_OPTIONS[node.__class__]}
        else:
            options = {}
        if node in self._options:
            options.update(self._options[node])
        return options

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
            in_qs = [edge_recs[edge.params] if edge is not None else None
                     for edge in G.indexed_in_edges(node.name)]
        return in_qs

    def get_quantized_dimension(self, node):
        if self._quantized_dimension == 'tensor':
            return None
        elif self._quantized_dimension == 'channel':
            return node.filter.get_order_idx('out_c')
        return None

    def calculate_q(self, G, node, astats, in_qs, dtype, out_dtype=None):
        if out_dtype is None:
            out_dtype = dtype
        if isinstance(node, (PoolingParameters, OutputParameters, SplitParameters)):
            o_q = in_qs[0]
        elif isinstance(node, SoftMaxParameters):
            o_q = SymmetricMultQType(min_val=-1, max_val=1, dtype=np.int16, scale=2**(-15))
        else:
            o_q = SymmetricMultQType.from_min_max(min_val=astats['range_out'][0]['min'],
                                                  max_val=astats['range_out'][0]['max'],
                                                  dtype=out_dtype)

        if isinstance(node, (MatrixAddParameters, MatrixSubParameters)):
            qrec = MultAddQuantizationRecord(in_qs=in_qs, out_qs=[o_q])

        elif isinstance(node, (MatrixBroadcastedLinearOpParameters, MatScaleFusionParameters, GlobalPoolParameters)):
            qrec = MultQuantizationRecord(in_qs=in_qs, out_qs=[o_q])

        elif isinstance(node, SplitParameters):
            qrec = MultQuantizationRecord(in_qs=in_qs, out_qs=[o_q]*node.num_splits)

        elif isinstance(node, ConstantInputParameters):
            if node.value_quantization:
                qrec = MultConstantQuantizationRecord(out_qs=[node.value_quantization],
                                                      constants_are_quantized=True)
            else:
                qrec = MultConstantQuantizationRecord(out_qs=[o_q],
                                                      constants_are_quantized=False)

        elif isinstance(node, (FcParameters, Conv2DParameters)):
            weights_q = SymmetricMultQType.from_array(arr=node.weights,
                                                      quantized_dimension=self.get_quantized_dimension(
                                                          node),
                                                      dtype=dtype, narrow_range=self._narrow_weights)
            if node.has_bias:
                biases_q = SymmetricMultBiasesQType(
                    dtype=np.int32, scale=weights_q.scale * in_qs[0].scale)
            else:
                biases_q = SymmetricMultBiasesQType(
                    dtype=np.int32, scale=np.array([1], dtype=np.int32))
            mul_biases_q = MultMulBiasScaleQType.from_filter(in_qs[0], weights_q, o_q, node)
            qrec = MultScalableFilterQuantizationRecord(in_qs=[in_qs[0]],
                                                        out_qs=[o_q],
                                                        weights_q=weights_q,
                                                        biases_q=biases_q,
                                                        mul_biases_q=mul_biases_q,
                                                        constants_are_quantized=False)
            LOG.debug("filter %s qrec %s", node.name, qrec)
        elif isinstance(node, RNNParameters):
            input_nodes = {RNNParameters.INPUT_NAMES[edge.to_idx]: edge.from_node
                           for edge in G.in_edges(node.name)
                           if isinstance(edge.from_node, ConstantInputParameters)}
            names = {val: idx for idx, val in enumerate(RNNParameters.INPUT_NAMES)}
            # quantization_mode: extended, autotiler
            # state_width: 16bit or 8bit
            opts = self.get_options(node)
            if opts['mode'] == "extended":
                in_w_scale = in_qs[names['i_2_i_w']].scale * in_qs[0].scale
                state_w_scale = in_qs[names['r_2_i_w']].scale
                i_2_a_q = MultMulBiasScaleQType(scale=in_w_scale/state_w_scale)
                s_2_s_q = MultMulBiasScaleQType(scale=state_w_scale)
                s_2_o_q = MultMulBiasScaleQType(scale=1/o_q.scale)
                self.rescale_constant(input_nodes['i_b'], state_w_scale, dtype=np.int32)
                qrec = MultScalableRnnQuantizationRecord(
                    in_qs=in_qs,
                    out_qs=[o_q],
                    i_2_a_q=i_2_a_q,
                    s_2_s_q=s_2_s_q,
                    s_2_o_q=s_2_o_q
                )
            elif opts['mode'] == 'autotiler':
                in_and_state_scale = np.maximum(in_qs[0].scale, o_q.scale)
                in_and_state_w_scale = np.maximum(
                    in_qs[names['i_2_i_w']].scale, in_qs[names['r_2_i_w']].scale)
                in_qs[0].scale = in_and_state_scale
                o_q.scale = in_and_state_scale
                self.rescale_constant(input_nodes['i_state'], in_and_state_scale)
                self.rescale_constant(input_nodes['i_2_i_w'], in_and_state_w_scale)
                self.rescale_constant(input_nodes['r_2_i_w'], in_and_state_w_scale)
                state_w_scale = in_and_state_scale * in_and_state_w_scale
                self.rescale_constant(input_nodes['i_b'], state_w_scale, dtype=np.int32)
                s_2_s_q = MultMulBiasScaleQType(scale=state_w_scale/in_and_state_scale)
                qrec = MultScalableRnnQuantizationRecord(
                    in_qs=in_qs,
                    out_qs=[o_q],
                    s_2_s_q=s_2_s_q,
                )
        elif isinstance(node, LSTMParameters):
            input_nodes = {LSTMParameters.INPUT_NAMES[edge.to_idx]: edge.from_node
                           for edge in G.in_edges(node.name)
                           if isinstance(edge.from_node, ConstantInputParameters)}
            names = {val: idx for idx, val in enumerate(LSTMParameters.INPUT_NAMES)}
            if node.cell_clip:
                cell_max = node.cell_clip
            else:
                cell_max = max(abs(astats['range_cell'][var]) for var in ['min', 'max'])

            cell_int_bits = calc_bits(cell_max)

            in_qs[names['c_state']].recalculate_scale(-cell_max,
                                                      cell_max)
            LOG.debug("cell bits %d max %d cell range %d",
                      cell_int_bits,
                      cell_max,
                      in_qs[names['c_state']].range)
            # worst case is (internal_q * 3) + 2 = 32 (1 for 1 and 1 for sign) i.e. 10
            # but also (internal_q * 2) + cell_bits = 32
            int_q = min((32-cell_int_bits)//2, 10)
            # in and out and state are all in the same scale
            in_and_out_scale = np.maximum(in_qs[0].scale, o_q.scale)
            in_and_state_scale = np.maximum(in_and_out_scale, in_qs[names['i_state']].scale)
            in_qs[0].scale = in_and_state_scale
            o_q.scale = in_and_state_scale
            self.rescale_constant(input_nodes['i_state'], in_and_state_scale)
            scale_pairs = {chan: ('i_2_%s_w' % chan, 'r_2_%s_w' % chan)
                           for chan in ['i', 'o', 'c', 'f']}
            scales = {k: np.maximum(in_qs[names[namei]].scale, in_qs[names[namer]].scale)
                      for k, (namei, namer) in scale_pairs.items()}
            for k, (namei, namer) in scale_pairs.items():
                self.rescale_constant(input_nodes[namei], scales[k])
                self.rescale_constant(input_nodes[namer], scales[k])
            int_scale = pow(2, -int_q)
            int2_scale = pow(2, -(int_q*2))
            int3_scale = pow(2, -(int_q*3))
            # compute scales for perceptrons
            pscales = {k: scales[k] * in_and_state_scale for k in ['i', 'o', 'c', 'f']}
            scale_qtypes = {"r_2_%s_q" % k: MultMulBiasScaleQType(
                scale=pscale/int_scale) for k, pscale in pscales.items()}
            scale_qtypes['cell_in_q'] = MultMulBiasScaleQType(
                scale=in_qs[names['c_state']].scale/int_scale)
            # TODO - Check cell clip here
            scale_qtypes['cell_out_q'] = MultMulBiasScaleQType(
                scale=int2_scale/in_qs[names['c_state']].scale)
            scale_qtypes['state_out_q'] = MultMulBiasScaleQType(scale=int3_scale/in_and_state_scale)
            # set internal scale
            scale_qtypes['i_qtype'] = QType(q=int_q, bits=32, signed=True)
            # set biases to output of perceptron
            for k in ['i', 'o', 'c', 'f']:
                self.rescale_constant(input_nodes["%s_b" % k], pscales[k], dtype=np.int32)
            qrec = MultScalableLstmQuantizationRecord(
                in_qs=in_qs,
                out_qs=[o_q],
                **scale_qtypes,
            )
        else:
            qrec = MultQuantizationRecord(in_qs=in_qs, out_qs=[o_q])
        return qrec

    def rescale_constant(self, node: ConstantInputParameters, scale, dtype=None):
        qrec = self.qrecs[NodeId(node)]
        qtype = qrec.out_qs[0]
        if (qtype.scale == scale.astype(qtype.scale.dtype) and
                (dtype is None or dtype == qtype.dtype)):
            return
        if node.value_quantization:
            value = node.value_quantization.dequantize(node.value)
            node.value = value
            qrec.constants_are_quantized = False

        qtype.scale = scale
        if dtype:
            qtype.dtype = dtype

    def quantize_fusion(self, G, node, in_qs, dtype):
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
            self.qrecs[NodeId(node, conv_node)] = conv_qrec
            fin_qs = conv_qrec.out_qs
            nodes = nodes[1:]
        for fnode in nodes:
            qrec = self.calculate_q(G,
                                    fnode,
                                    self._activation_stats.get(NodeId(node, fnode)),
                                    fin_qs,
                                    dtype)
            self.qrecs[NodeId(node, fnode)] = qrec
            fin_qs = qrec.out_qs
        return MultQuantizationRecord(in_qs=in_qs, out_qs=fin_qs)

    def quantize_forward(self, G: NNGraph, edge_recs, dtype=np.int8):
        for node in [step['node'] for step in G.graph_state.steps]:
            LOG.debug("quantize forward %s", node.name)
            in_qs = self.get_in_qs(G, edge_recs, node)
            if isinstance(node, (ConvFusionParameters, ActivationFusion)):
                qrec = self.quantize_fusion(G, node, in_qs, dtype)
            else:
                qrec = self.calculate_q(G,
                                        node,
                                        self._activation_stats.get(
                                            NodeId(node, None)),
                                        in_qs,
                                        dtype)
            self.qrecs[NodeId(node, None)] = qrec
            if not qrec:
                break

            for edges in G.indexed_out_edges(node.name):
                for edge in edges:
                    edge_recs[edge.params] = qrec.out_qs[edge.from_idx]

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
        self.qrecs = QuantizationSet()
        edge_recs = {}
        dtype = WIDTH_TO_DTYPE[self._force_width]
        self.quantize_forward(G, edge_recs, dtype)
        self.qrecs['__quantizer'] = self
        G.graph_identity.quantization_type = 'SQ8'
        return self.qrecs
