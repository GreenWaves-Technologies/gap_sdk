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

from functools import reduce
import logging

import numpy as np

from quantization.multiplicative.mult_quantization import \
    MultQuantizationRecord
from quantization.quantization_record_base import QuantizationRecordBase
from utils.at_norm import at_norm

LOG = logging.getLogger("nntool." + __name__)

# pylint: disable=too-many-arguments, too-many-locals


def av_pool(params,
            in_tensors,
            qrec: QuantizationRecordBase,
            details=None):
    del details
    # Prepare the quantization levels

    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    in_dims = params.in_dims[0]
    out_dims = params.out_dims[0]
    filter_sz = params.filter.h * params.filter.w

    pool_factor = (1 << 16)//filter_sz

    out_tensor = np.zeros(out_dims.shape, dtype=np.int32)

    if params.padding.h + params.padding.w > 0:
        in_tensor = np.pad(in_tensor,
                           params.padding.numpy_pad_shape(in_dims),
                           mode='constant',
                           constant_values=qrec.in_qs[0].pad_zero_point)
        pad_w = params.padding.w
        pad_h = params.padding.h
    else:
        pad_w = pad_h = 0

    for in_c in range(out_dims.c):

        out_h = 0
        for h_idx in range(0, in_dims.h - params.filter.h + pad_h + 1,
                           params.stride.h):
            out_w = 0
            for w_idx in range(0, in_dims.w - params.filter.w + pad_w + 1,
                               params.stride.w):
                # accumulate - potentially with different Q
                in_slice_args = in_dims.srange(c=[in_c, in_c + 1, 1],
                                               h=[h_idx, h_idx + params.filter.h, 1],
                                               w=[w_idx, w_idx + params.filter.w, 1])

                sum_filter = np.sum(in_tensor[in_slice_args], dtype=np.int32)
                sum_filter = np.multiply(sum_filter, pool_factor, dtype=np.int32)
                out_tensor[out_dims.srange(c=in_c, h=out_h, w=out_w)] = sum_filter
                out_w += 1
            out_h += 1

    return qrec.get_outputs(params, [qrec.out_qs[0].clip(at_norm(out_tensor, 16), qrec.out_qs[0].dtype)], ktype="symmetric")


def max_pool(params,
             in_tensors,
             qrec: QuantizationRecordBase,
             details=None):

    del details
    # Prepare the quantization levels

    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    in_dims = params.in_dims[0]
    out_dims = params.out_dims[0]

    out_tensor = np.zeros(out_dims.shape, dtype=qrec.out_qs[0].dtype)

    if params.padding.h + params.padding.w > 0:
        in_tensor = np.pad(in_tensor,
                           params.padding.numpy_pad_shape(in_dims),
                           mode='constant',
                           constant_values=qrec.in_qs[0].pad_zero_point)
        pad_w = params.padding.w
        pad_h = params.padding.h
    else:
        pad_w = pad_h = 0

    for in_c in range(out_dims.c):
        out_h = 0
        for h_idx in range(0, in_dims.h - params.filter.h + pad_h + 1,
                           params.stride.h):
            out_w = 0
            for w_idx in range(0, in_dims.w - params.filter.w + pad_w + 1,
                               params.stride.w):
                # accumulate - potentially with different Q
                out_slice_args = out_dims.srange(c=in_c, h=out_h, w=out_w)
                in_slice_args = in_dims.srange(c=[in_c, in_c + 1, 1],
                                               h=[h_idx, h_idx + params.filter.h, 1],
                                               w=[w_idx, w_idx + params.filter.w, 1])

                out_tensor[out_slice_args] = np.max(in_tensor[in_slice_args].view(np.ndarray))
                out_w += 1
            out_h += 1

    return qrec.get_outputs(params, [out_tensor], ktype="symmetric")


def gap_clb(sum_):
    '''Count Leading 0s or 1s'''
    sum_bin = [np.binary_repr(sum_elem, width=32) for sum_elem in sum_]
    return [len(s) - len(s.lstrip(s[0])) - 1 for s in sum_bin]


def av_global_pool_mult(params,
                        in_tensors,
                        qrec: MultQuantizationRecord,
                        details=None):

    # Prepare the quantization levels
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    out_dims = params.out_dims[0]
    qrec.set_scale(in_idx=0, out_idx=0)

    sum_by_chan = np.sum(in_tensor, dtype=np.int32, axis=tuple(
        params.axis), keepdims=params.keep_dims)
    sz = reduce(lambda x, y: x * y, [i for idx,
                                     i in enumerate(in_tensor.shape) if idx in params.axis])
    res = at_norm((sum_by_chan << 7) // sz, 7)
    res = out_tensor = qrec.scale_mul_biases_q.apply_scales(res)
    return qrec.get_outputs(params,
                            [out_tensor.reshape(out_dims.shape)],
                            ktype="symmetric")


def av_global_pool(params,
                   in_tensors,
                   qrec: QuantizationRecordBase,
                   details=None):

    if isinstance(qrec, MultQuantizationRecord):
        return av_global_pool_mult(params, in_tensors, qrec, details=details)

    # Prepare the quantization levels
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    out_dims = params.out_dims[0]

    sum_by_chan = np.sum(in_tensor, dtype=np.int32, axis=tuple(
        params.axis), keepdims=params.keep_dims)

    norm = (np.array([31], dtype=np.int32) - gap_clb(sum_by_chan.flatten())).astype(np.int32)
    sz = reduce(lambda x, y: x * y, [i for idx,
                                     i in enumerate(in_tensor.shape) if idx in params.axis])
    inv_wh = ((1 << norm) // sz).reshape(sum_by_chan.shape)
    out_tensor = at_norm((inv_wh * sum_by_chan), norm.reshape(sum_by_chan.shape))
    return qrec.get_outputs(params,
                            [qrec.out_qs[0].clip(out_tensor).reshape(out_dims.shape)],
                            ktype="symmetric")


def max_global_pool(params,
                    in_tensors,
                    qrec: QuantizationRecordBase,
                    details=None):

    del details

    # Prepare the quantization levels
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    if isinstance(qrec, MultQuantizationRecord):
        qrec.set_scale(in_idx=0, out_idx=0)
    return qrec.get_outputs(params, [np.max(in_tensor,
                                            axis=tuple(params.axis),
                                            keepdims=params.keep_dims)], ktype="symmetric")


def sum_global_pool(params,
                    in_tensors,
                    qrec: QuantizationRecordBase,
                    details=None):
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]
    if isinstance(qrec, MultQuantizationRecord):
        qrec.set_scale(in_idx=0, out_idx=0)
    res = np.sum(in_tensor,
                 axis=tuple(params.axis),
                 keepdims=params.keep_dims,
                 dtype=np.int32)

    if isinstance(qrec, MultQuantizationRecord):
        res = qrec.scale_mul_biases_q.apply_scales(res)
    return qrec.get_outputs(params, [res], ktype="symmetric")
