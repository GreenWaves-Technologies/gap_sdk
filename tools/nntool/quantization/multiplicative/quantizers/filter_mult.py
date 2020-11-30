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

import numpy as np
from graph.types import Conv2DParameters, FcParameters
from quantization.multiplicative.mult_quantization import \
    MultScalableFilterQuantizationRecord
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import \
    MultMulBiasScaleQType
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import \
    SymmetricMultBiasesQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.quantization_handler import can_dequantize, params_type
from utils.node_id import NodeId

from ..mult_quantization_handler import MultQuantizionHandler


@params_type(FcParameters, Conv2DParameters)
@can_dequantize(True)
class FilterMult(MultQuantizionHandler):
    @classmethod
    def get_quantized_dimension(cls, params, opts):
        if opts['quantized_dimension'] == 'tensor':
            return None
        elif opts['quantized_dimension'] == 'channel':
            return params.filter.get_order_idx('out_c')
        return None

    @classmethod
    def _quantize(cls, params, in_qs, out_dtype, stats, **kwargs):
        opts = kwargs['opts']
        fusion = kwargs.get('fusion', None)
        if fusion and fusion.fusion_type in ['conv_active_pool', 'conv_active']:
            # Take stats from activation after the convolution
            stats = kwargs['all_stats'][NodeId(fusion, fusion.contained_nodes()[1])]
        o_q = SymmetricMultQType.from_min_max(min_val=stats['range_out'][0]['min'],
                                              max_val=stats['range_out'][0]['max'],
                                              dtype=out_dtype)
        weights_q = SymmetricMultQType.from_array(arr=params.weights,
                                                  quantized_dimension=cls.get_quantized_dimension(
                                                      params, opts),
                                                  dtype=np.int8, narrow_range=opts['narrow_weights'])
        if params.has_bias:
            biases_q = SymmetricMultBiasesQType(
                dtype=np.int32, scale=weights_q.scale * in_qs[0].scale)
        else:
            biases_q = SymmetricMultBiasesQType(
                dtype=np.int32, scale=np.array([1], dtype=np.int32))
        mul_biases_q = MultMulBiasScaleQType.from_filter(in_qs[0], weights_q, o_q, params)
        return MultScalableFilterQuantizationRecord(in_qs=[in_qs[0]],
                                                    out_qs=[o_q],
                                                    weights_q=weights_q,
                                                    biases_q=biases_q,
                                                    mul_biases_q=mul_biases_q,
                                                    constants_are_quantized=False)

    @classmethod
    def _dequantize(cls, params, qrec):
        if params.has_bias:
            params.biases = qrec.biases_q.dequantize(params.biases)
        params.weights = qrec.weights_q.dequantize(params.weights)
