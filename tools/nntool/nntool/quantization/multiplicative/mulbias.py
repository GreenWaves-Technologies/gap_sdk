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

import math
from functools import reduce
from typing import Sequence

import numpy as np
from nntool.graph.types import FilterNodeBase, MultiplicativeBiasNodeBase
from nntool.graph.types.base import NNNodeBase
from nntool.quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from nntool.quantization.new_qrec import QRec

from nntool.utils.at_norm import at_norm


def compute_prenorm(qrec, params: FilterNodeBase):
    if not qrec.cache.get('enable_prenorm'):
        return 0
    max_bits = qrec.in_qs[0].bits - 1 + qrec.in_qs[1].bits - 1 + 1 + \
        math.ceil(math.log2(params.filter_dim.in_c *
                            params.filter_dim.h * params.filter_dim.w))
    spare_bits = 31 - max_bits
    mul_biases_q = qrec.cache.get('mul_biases_q')
    if not mul_biases_q:
        raise ValueError(f'no mul_bias configuration found for {params.name}')
    if mul_biases_q.dtype == np.int8:
        bits = 7
    elif mul_biases_q.dtype == np.uint8:
        bits = 8
    else:
        raise ValueError(f"incorrect dtype for mulbiases for {params.name}")
    return max(0, bits - spare_bits)


def get_quantized_bias_offset(qrec, params, weights):
    # input zero correction is sum(W * Zin) by out_c if weights are channel scaled
    axis = tuple([idx for idx in range(4) if idx !=
                  params.filter_dim.get_order_idx('out_c')])
    return np.sum(np.multiply(qrec.in_qs[0].zero_point.astype(np.int32),
                              weights,
                              dtype=np.int32),
                  dtype=np.int32,
                  axis=axis)


def biases_zero_correction(qrec):
    # output zero correction is So/(Si * Sw) * ZPo by out_c if weights are channel scaled
    scale = qrec.out_qs[0].scale / (qrec.in_qs[0].scale * qrec.in_qs[1].scale)
    return np.floor((qrec.out_qs[0].zero_point.astype(np.int32) * scale) + 0.5).astype(np.int32)


def gen_mul_biases(qrec, params: MultiplicativeBiasNodeBase) -> np.ndarray:
    mul_biases_q = qrec.cache.get('mul_biases_q')
    if mul_biases_q is None:
        raise ValueError(f'no mul_bias configuration found for {params.name}')
    return mul_biases_q.qbiases


def apply_multiplicative_bias(qrec, params: FilterNodeBase, input_tensor: np.ndarray,
                              axis: int, ktype: str = None):
    if ktype == 'float':
        if hasattr(params, 'has_mul_bias') and params.has_mul_bias:
            shape = [params.filter_dim.out_c if idx ==
                     axis else 1 for idx in range(3)]
            input_tensor *= params.mul_biases.reshape(shape)
        return input_tensor
    if ktype == 'symmetric' and qrec.ktype.startswith('scaled'):
        mul_biases_q = qrec.cache.get('mul_biases_q')
        if isinstance(mul_biases_q, MultMulBiasScaleQType):
            input_tensor = mul_biases_q.apply_scales(input_tensor, axis)
    elif ktype == 'symmetric' and qrec.ktype.startswith('symmetric'):
        if params.has_mul_bias:
            mul_biases_q = qrec.cache.get('mul_biases_q')
            mul_biases = mul_biases_q.quantize(params.mul_biases)
            shape = [params.filter_dim.out_c if idx ==
                     axis else 1 for idx in range(3)]
            input_tensor *= mul_biases.reshape(shape)
            input_tensor = at_norm(input_tensor, mul_biases_q.q)
    return input_tensor.astype(np.int32)


def apply_zero_offset_bias(qrec: QRec, params: NNNodeBase,
                           input_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
    if ktype == 'symmetric':
        if qrec.in_qs[2].offset is None:
            if qrec.in_qs[0].zero_point != 0:
                input_tensors[2] -= get_quantized_bias_offset(
                    qrec, params, input_tensors[1])
            if qrec.out_qs[0].zero_point != 0:
                input_tensors[2] += biases_zero_correction(qrec)
    return input_tensors


def compute_in_out_scale(qrec, in_idx=0, out_idx=0, extra_scale=1):
    if isinstance(in_idx, int):
        in_scale = qrec.in_qs[in_idx].scale
    else:
        in_scale = reduce(lambda x, y: x * y,
                          [qrec.in_qs[idx].scale for idx in in_idx])
    if isinstance(out_idx, int):
        out_scale = qrec.out_qs[out_idx].scale
    else:
        out_scale = reduce(lambda x, y: x * y,
                           [qrec.out_qs[idx].scale for idx in out_idx])
    scale_mul_biases_q = qrec.cache.get('scale_mul_biases_q')
    if scale_mul_biases_q is None:
        scale_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
        qrec.cache['scale_mul_biases_q'] = scale_mul_biases_q

    scale = in_scale * extra_scale / out_scale
    scale_mul_biases_q.scale = scale


def set_add_in_scale(qrec):
    scaled_idx = qrec.cache.get('scaled_idx')
    if scaled_idx is None:
        scaled_idx = (1 if qrec.in_qs[1].scale > qrec.in_qs[0].scale else 0)
        qrec.cache['scaled_idx'] = scaled_idx
    compute_in_out_scale(qrec, in_idx=0 if scaled_idx else 1)
    scale_in_mul_biases_q = qrec.cache.get('scale_in_mul_biases_q')
    if scale_in_mul_biases_q is None:
        scale_in_mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
        qrec.cache['scale_in_mul_biases_q'] = scale_in_mul_biases_q
    not_scaled_idx = 0 if scaled_idx else 1
    scale = qrec.in_qs[scaled_idx].scale / qrec.in_qs[not_scaled_idx].scale
    scale_in_mul_biases_q.scale = scale

    if qrec.in_qs[0].asymmetric:
        # (C - Zc)*Sc = (A - Za)*Sa + (B - Zb)*Sb =
        # C = Sa/Sc*(A + B*Sb/Sa - Za - Zb*Sb/Sa) + Zc =
        #   = Sa/Sc*(A + B*Sb/Sa) + (Zc - Sa/Sc*(Za + Zb*Sb/Sa))
        #                           |---------- bias ----------|
        add_bias = qrec.out_qs[0].zero_point - qrec.cache['scale_mul_biases_q'].scale * (
            qrec.in_qs[not_scaled_idx].zero_point + scale_in_mul_biases_q.scale * qrec.in_qs[scaled_idx].zero_point)
    else:
        add_bias = 0
    qrec.cache['add_bias_offset'] = np.round(add_bias).astype(np.int16)

def set_ssd_scales(qrec, params):
    offset_q = qrec.in_qs[0]
    anchors_q = qrec.in_qs[2]
    out_boxes_q = qrec.out_qs[0]
    for k in ['scale_x_q', 'scale_x_anc_q',
              'scale_y_q', 'scale_y_anc_q',
              'scale_h_q', 'scale_w_q',
              'scale_ao_q']:
        if k not in qrec.cache:
            qrec.cache[k] = MultMulBiasScaleQType(dtype=np.uint8)

    qrec.cache['scale_x_q'].scale = (offset_q.scale * anchors_q.scale) / \
        (out_boxes_q.scale * params.x_scale)
    qrec.cache['scale_x_anc_q'].scale = params.x_scale / offset_q.scale
    qrec.cache['scale_y_q'].scale = (offset_q.scale * anchors_q.scale) / \
        (out_boxes_q.scale * params.y_scale)
    qrec.cache['scale_y_anc_q'].scale = params.y_scale / offset_q.scale
    qrec.cache['scale_h_q'].scale = offset_q.scale / params.h_scale
    qrec.cache['scale_w_q'].scale = offset_q.scale / params.w_scale
    qrec.cache['scale_ao_q'].scale = anchors_q.scale * \
        2**(-15) / out_boxes_q.scale
