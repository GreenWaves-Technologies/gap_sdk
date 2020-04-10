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

import numpy as np

from graph.dim import Dim
from quantization.quantization_record import FilterQuantizationRecord

from .utils import pad, prepare_acc, srange

LOG = logging.getLogger("nntool." + __name__)

# pylint: disable=invalid-name
def faster_conv(params, in_dims: Dim, out_dims: Dim, in_tensor: np.ndarray,
                weights: np.ndarray, biases: np.ndarray, mul_biases: np.ndarray, details):
    '''3D convolution by sub-matrix summing.
    '''
    if details is not None:
        details['min_acc'] = float("Infinity")
        details['max_acc'] = float("-Infinity")

    in_tensor = in_tensor.transpose(in_dims.transpose_to_order(['h', 'w', 'c']))
    if params.padding.h + params.padding.w > 0:
        in_tensor = np.pad(in_tensor,
                           ([params.padding.t,
                             params.padding.b],
                            [params.padding.l,
                             params.padding.r])\
                               + ([0, 0], ) * (np.ndim(in_tensor)-2),
                           mode='constant',
                           constant_values=0)
        pad_w = params.padding.w
        pad_h = params.padding.h
    else:
        pad_w = pad_h = 0

    weights = weights.copy()
    weights = weights.transpose(params.filter.transpose_to_order(['out_c', 'h', 'w', 'in_c']))

    filt_w = params.filter.w
    filt_h = params.filter.h

    in_w = in_dims.w
    in_h = in_dims.h
    out_c = params.filter.out_c

    in_c_per_group = in_dims.c // params.groups
    out_c_per_group = out_c // params.groups
    in_c_off = 0
    out_c_cnt = 0

    out_w = ((in_w - filt_w + pad_w))+1
    out_h = ((in_h - filt_h + pad_h))+1
    if biases is None:
        result = np.zeros((out_c, out_h, out_w))
    else:
        result = np.ones((out_c, out_h, out_w)) * biases.reshape(out_c, 1, 1)

    const_h = pad_h + in_h - filt_h + 1
    const_w = pad_w + in_w - filt_w + 1
    for out_c_i in range(out_dims.c):
        for cur_h in range(filt_h):
            for cur_w in range(filt_w):
                # selects all elements that the filter element needs to multiply
                slabhw = in_tensor[cur_h:
                                   const_h + cur_h:
                                   1,
                                   cur_w:
                                   const_w + cur_w:
                                   1,
                                   in_c_off:
                                   in_c_off + in_c_per_group:
                                   1] * weights[out_c_i, cur_h, cur_w]
                # add depthwise
                slabhw = slabhw.sum(axis=-1)
                # add to the previous filter elements
                result[out_c_i] += slabhw

                if details is not None:
                    details['min_acc'] = min(np.min(result[out_c_i]), details['min_acc'])
                    details['max_acc'] = max(np.max(result[out_c_i]), details['max_acc'])
        out_c_cnt += 1
        if out_c_cnt >= out_c_per_group:
            out_c_cnt = 0
            in_c_off += in_c_per_group

    if params.stride.size() > 1:
        result = result[:, ::params.stride.h, ::params.stride.w, ...]

    if params.has_mul_bias:
        result *= mul_biases.reshape(out_c, 1, 1)

    return result.transpose(out_dims.transpose_from_order(['c', 'h', 'w']))

# pylint: disable=invalid-name
def faster_conv_quantized(params,
                          qrec: FilterQuantizationRecord,
                          in_dims: Dim,
                          out_dims: Dim,
                          in_tensor: np.ndarray,
                          weights: np.ndarray,
                          biases: np.ndarray,
                          mul_biases: np.ndarray,
                          details,
                          detect_overflow=True):
    '''3D convolution by sub-matrix summing.
    '''
    if details is not None:
        details['min_acc'] = float("Infinity")
        details['max_acc'] = float("-Infinity")
        details['overflow_dot'] = 0
        details['overflow_acc'] = 0

    in_tensor = in_tensor.transpose(in_dims.transpose_to_order(['h', 'w', 'c']))
    if params.padding.h + params.padding.w > 0:
        in_tensor = np.pad(in_tensor,
                           ([params.padding.t,
                             params.padding.b],
                            [params.padding.l,
                             params.padding.r])\
                               + ([0, 0], ) * (np.ndim(in_tensor)-2),
                           mode='constant',
                           constant_values=0)
        pad_w = params.padding.w
        pad_h = params.padding.h
    else:
        pad_w = pad_h = 0

    in_tensor = in_tensor.astype(qrec.calc_q.dtype)

    weights = weights.transpose(params.filter.transpose_to_order(['out_c', 'h', 'w', 'in_c']))

    filt_w = params.filter.w
    filt_h = params.filter.h

    in_w = in_dims.w
    in_h = in_dims.h
    out_c = params.filter.out_c

    in_c_per_group = in_dims.c // params.groups
    out_c_per_group = out_c // params.groups
    in_c_off = 0
    out_c_cnt = 0

    out_w = ((in_w - filt_w + pad_w))+1
    out_h = ((in_h - filt_h + pad_h))+1
    if biases is None:
        result = np.zeros((out_c, out_h, out_w), dtype=qrec.acc_q.dtype)
    else:
        if qrec.acc_q != qrec.biases_q:
            biases = qrec.acc_q.expand_from(biases, qrec.biases_q)
        result = np.ones((out_c, out_h, out_w), dtype=qrec.acc_q.dtype) *\
            biases.reshape(out_c, 1, 1)

    if detect_overflow:
        result64 = result.astype(np.int64)

    const_h = pad_h + in_h - filt_h + 1
    const_w = pad_w + in_w - filt_w + 1
    for out_c_i in range(out_dims.c):
        for cur_h in range(filt_h):
            for cur_w in range(filt_w):
                if detect_overflow:
                    # selects all elements that the filter element needs to multiply
                    slabhw64 = in_tensor[cur_h:
                                         const_h + cur_h:
                                         1,
                                         cur_w:
                                         const_w + cur_w:
                                         1,
                                         in_c_off:
                                         in_c_off + in_c_per_group:
                                         1].astype(np.int64) * weights[out_c_i, cur_h, cur_w]
                    if qrec.calc_q != qrec.acc_q:
                        # reduce the accumulator
                        slabhwpost = qrec.acc_q.round_normalize_clip(slabhw64,
                                                                     qrec.calc_q,
                                                                     change_type=False)
                    else:
                        slabhwpost = slabhw64
                    # add depthwise
                    slabhw64sum = slabhwpost.sum(axis=-1)
                    # add to the previous filter elements
                    result64[out_c_i] += slabhw64sum

                # selects all elements that the filter element needs to multiply
                slabhw = in_tensor[cur_h:
                                   const_h + cur_h:
                                   1,
                                   cur_w:
                                   const_w + cur_w:
                                   1,
                                   in_c_off:
                                   in_c_off + in_c_per_group:
                                   1] * weights[out_c_i, cur_h, cur_w]

                if detect_overflow:
                    if np.any(slabhw < slabhw64):
                        details['overflow_dot'] += 1

                if qrec.calc_q != qrec.acc_q:
                    # reduce the accumulator
                    slabhw = qrec.acc_q.reduce_from(slabhw, qrec.calc_q)

                # add depthwise
                slabhw = slabhw.sum(axis=-1)
                # add to the previous filter elements
                if detect_overflow:
                    acc_overflow_detected = False
                    if np.any(slabhw != slabhw64sum):
                        details['overflow_acc'] += 1
                        acc_overflow_detected = True

                result[out_c_i] += slabhw
                if detect_overflow and not acc_overflow_detected:
                    if np.any(result[out_c_i] != result64[out_c_i]):
                        details['overflow_acc'] += 1

                if details is not None:
                    details['min_acc'] = min(np.min(result[out_c_i]), details['min_acc'])
                    details['max_acc'] = max(np.max(result[out_c_i]), details['max_acc'])

        out_c_cnt += 1
        if out_c_cnt >= out_c_per_group:
            out_c_cnt = 0
            in_c_off += in_c_per_group

    if params.stride.size() > 1:
        result = result[:, ::params.stride.h, ::params.stride.w, ...]

    if qrec.out_qs[0] != qrec.acc_q:
        result = qrec.out_qs[0].reduce_from(result, qrec.acc_q)

    if params.has_mul_bias:
        result = result.astype(qrec.calc_q.dtype)
        result *= mul_biases.reshape(out_c, 1, 1)
        result = qrec.out_qs[0].reduce_from(result, qrec.out_qs[0] + qrec.mul_biases_q)

    return result.transpose(out_dims.transpose_from_order(['c', 'h', 'w']))

# filter OIHW
# pylint: disable=too-many-locals, too-many-branches, too-many-arguments
def conv2d(params,
           in_dims: Dim,
           out_dims: Dim,
           in_tensor: np.ndarray,
           weights: np.ndarray,
           biases: np.ndarray,
           mul_biases: np.ndarray = None,
           qrec=None,
           details=None,
           allow_faster=True):

    if allow_faster and params.dilation.size() == 1:
        if qrec:
            return faster_conv_quantized(params,
                                         qrec,
                                         in_dims,
                                         out_dims,
                                         in_tensor,
                                         weights,
                                         biases,
                                         mul_biases,
                                         details)
        else:
            return faster_conv(params,
                               in_dims,
                               out_dims,
                               in_tensor,
                               weights,
                               biases,
                               mul_biases,
                               details)

    assert mul_biases is None, "mulbiases not supported in dilated conv"

    acc_tensor = prepare_acc(biases, out_dims, qrec)

    if details is not None:
        details['min_acc'] = float("Infinity")
        details['max_acc'] = float("-Infinity")
        details['overflow_dot'] = 0
        details['overflow_acc'] = 0

    # undilated filter size
    filter_sz1 = params.filter.h * params.filter.w
    # dilated filter size
    filter_sz = params.filter + (params.filter - 1) * (params.dilation - 1)
    if params.padding.h + params.padding.w > 0:
        in_tensor = pad(in_tensor, in_dims, params.padding, params.pad_type)
    if qrec:
        # force the bit dimension of the input tensor to the bit width of the calc
        # so that the dot product occurs in this precision
        in_tensor = in_tensor.astype(qrec.calc_q.dtype)

    if params.tf_depthwise:
        filters_per_out_c = 1
    else:
        filters_per_out_c = params.filter.in_c // params.groups

    mult_idx = 0
    in_c_off = 0
    grp_idx = 0
    for out_c in range(out_dims.c):
        for in_c in range(in_c_off, in_c_off + filters_per_out_c, 1):
            if qrec and qrec.calc_q != qrec.acc_q:
                # Expand and normalize the accumulator
                acc_tensor = qrec.calc_q.expand_from(acc_tensor, qrec.acc_q)
            # init filter
            if params.tf_depthwise:
                w_slice_args = srange(params.filter,
                                      out_c=out_c,
                                      in_c=0)
            else:
                w_slice_args = srange(params.filter,
                                      out_c=out_c,
                                      in_c=in_c)
            w_slice = weights[w_slice_args].ravel()
            process_input(in_dims, filter_sz, params, out_dims, out_c, in_c,
                          in_tensor, w_slice, acc_tensor,
                          qrec, details)

            if qrec and qrec.calc_q != qrec.acc_q:
                # reduce the accumulator
                acc_tensor = qrec.acc_q.reduce_from(acc_tensor, qrec.calc_q)

        if params.multiplier > 1:
            mult_idx = mult_idx + 1
            if mult_idx >= params.multiplier:
                in_c_off += 1
                mult_idx = 0
        else:
            if params.groups > 1:
                grp_idx += 1
                if grp_idx >= filters_per_out_c:
                    in_c_off += filters_per_out_c
                    grp_idx = 0

    if qrec and qrec.out_qs[0] != qrec.acc_q:
        return qrec.out_qs[0].reduce_from(acc_tensor, qrec.acc_q)
    return acc_tensor

def process_input(in_dims, filter_sz, params, out_dims, out_c,
                  in_c, in_tensor, w_slice, acc_tensor,
                  qrec, details):
    out_h = 0
    for h_idx in range(0, in_dims.h - filter_sz.h + params.padding.h + 1,
                       params.stride.h):
        out_w = 0
        for w_idx in range(0, in_dims.w - filter_sz.w + params.padding.w + 1,
                           params.stride.w):

            acc_slice_args = srange(out_dims, c=out_c, h=out_h, w=out_w)
            in_slice_args =\
                srange(in_dims,
                       c=[in_c, in_c + 1, 1],
                       h=[h_idx, h_idx + filter_sz.h, params.dilation.h],
                       w=[w_idx, w_idx + filter_sz.w, params.dilation.w])
            in_slice = in_tensor[in_slice_args].ravel()

            res = np.dot(in_slice, w_slice)
            if qrec:
                res64 = np.dot(in_slice.astype(np.int64), w_slice.astype(np.int64))
                if res64 != res:
                    has_warned = True
                    if details is not None:
                        details['overflow_dot'] += 1
                    else:
                        LOG.warning(
                            "overflow in conv dot product InC=%d OutC=%d H=%d W=%d",
                            in_c, out_c, h_idx, w_idx)
                else:
                    has_warned = False
                acc64 = acc_tensor[acc_slice_args].astype(np.int64)
            acc_slice = acc_tensor[acc_slice_args]
            acc_slice += res
            if qrec:
                if acc64 + res64 != acc_slice:
                    if details is not None:
                        details['overflow_acc'] += 1
                    elif not has_warned:
                        LOG.warning(
                            "accumulation overflow in conv InC=%d OutC=%d H=%d W=%d",
                            in_c, out_c, h_idx, w_idx)
            elif details is not None:
                details['min_acc'] = min(np.min(acc_slice), details['min_acc'])
                details['max_acc'] = max(np.max(acc_slice), details['max_acc'])
            out_w += 1
        out_h += 1
