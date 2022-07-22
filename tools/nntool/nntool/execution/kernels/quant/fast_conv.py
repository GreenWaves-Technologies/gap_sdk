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
# from numba import njit, prange
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.graph.types import Conv2DNode
from nntool.quantization.multiplicative.mulbias import apply_multiplicative_bias
from nntool.quantization.new_qrec import QRec
from nntool.utils.numpy_fast_conv import fast_numpy_conv

FORCE_INT64 = False

# pylint: disable=invalid-name

LOG = logging.getLogger(__name__)

prange = range

def do_conv_with_reduction(details, in_tensor, weights, acc_q, calc_q,
                           filt_w, filt_h, filt_dil_h, filt_dil_w, filt_str_h, filt_str_w, out_c, in_c_per_group,
                           out_c_per_group, in_c_off, out_c_cnt, result, const_h, const_w, calc_dtype):
    for out_c_i in range(out_c):
        for cur_h in range(filt_h):
            for cur_w in range(filt_w):
                # selects all elements that the filter element needs to multiply
                slabhw = np.multiply(in_tensor[cur_h * filt_dil_h:
                                               const_h + cur_h * filt_dil_h:
                                               filt_str_h,
                                               cur_w * filt_dil_w:
                                               const_w + cur_w * filt_dil_w:
                                               filt_str_w,
                                               in_c_off:
                                               in_c_off + in_c_per_group:
                                               1],
                                     weights[out_c_i, cur_h, cur_w],
                                     dtype=calc_dtype)

                slabhw = acc_q.reduce_from(slabhw, calc_q)

                # add depthwise
                slabhw = slabhw.sum(
                    axis=-1, dtype=calc_dtype)
                # add to the previous filter elements
                result[out_c_i] += slabhw

                if details is not None:
                    details['min_acc'] = min(
                        np.min(result[out_c_i]), details['min_acc'])
                    details['max_acc'] = max(
                        np.max(result[out_c_i]), details['max_acc'])

        out_c_cnt += 1
        if out_c_cnt >= out_c_per_group:
            out_c_cnt = 0
            in_c_off += in_c_per_group


# @njit(parallel=True)
def do_conv(min_acc, max_acc, in_tensor, weights,
            filt_w, filt_h, filt_dil_h, filt_dil_w,
            filt_str_h, filt_str_w,
            out_c, in_c_per_group, out_c_per_group, in_c_off,
            out_c_cnt, result,
            const_h, const_w, calc_dtype):
    in_tensor = in_tensor.astype(calc_dtype)
    weights = weights.astype(calc_dtype)
    #pylint: disable=not-an-iterable
    for out_c_i in prange(out_c):
        for cur_h in range(filt_h):
            for cur_w in range(filt_w):
                # selects all elements that the filter element needs to multiply
                slabhw = np.multiply(in_tensor[cur_h * filt_dil_h:
                                               const_h + cur_h * filt_dil_h:
                                               filt_str_h,
                                               cur_w * filt_dil_w:
                                               const_w + cur_w * filt_dil_w:
                                               filt_str_w,
                                               in_c_off:
                                               in_c_off + in_c_per_group:
                                               1],
                                     weights[out_c_i, cur_h, cur_w])
                slabhw = slabhw.astype(calc_dtype)
                # add depthwise
                slabhw = slabhw.sum(
                    axis=-1)
                # add to the previous filter elements
                result[out_c_i] += slabhw

                min_acc = min(
                    np.min(result[out_c_i]), min_acc)
                max_acc = max(
                    np.max(result[out_c_i]), max_acc)

        out_c_cnt += 1
        if out_c_cnt >= out_c_per_group:
            out_c_cnt = 0
            in_c_off += in_c_per_group
    return min_acc, max_acc


@params_type(Conv2DNode)
@qrec_type('symmetric', 'scaled', 'scaled_ne16')
class Conv2DSymmetric(KernelBase):
    @classmethod
    def execute(cls, params: Conv2DNode,
                in_tensors,
                qrec: QRec,
                **kwargs):
        '''3D convolution by sub-matrix summing.
        '''
        details = kwargs.get('details')

        in_dims, w_dims, out_dims = params.in_dims[0], params.filter_dim, params.out_dims[0]
        prepared_in_tensors = qrec.prepare_inputs(
            params, in_tensors, ktype="symmetric")
        in_tensor = prepared_in_tensors[0]
        # expand the weights to apply the zero offset
        weights = prepared_in_tensors[1].astype(
            np.int32) - qrec.in_qs[1].zero_point.astype(np.int32)
        # if zero offset is already applied in biases by constant quantizer this does nothing
        biases = prepared_in_tensors[2]

        acc_q = qrec.cache.get('acc_q') or qrec.in_qs[2]
        calc_q = qrec.cache.get('calc_q') or qrec.in_qs[2]

        if details is not None:
            details['min_acc'] = float("Infinity")
            details['max_acc'] = float("-Infinity")

        in_tensor = in_tensor.transpose(
            in_dims.transpose_to_order(['h', 'w', 'c']))
        if params.padding.h + params.padding.w > 0:
            if hasattr(qrec.in_qs[0], 'zero_point'):
                const_pad = qrec.in_qs[0].zero_point[0].astype(np.int32)
            else:
                const_pad = np.int32(0)
            in_tensor = np.pad(in_tensor,
                               ([params.padding.t,
                                 params.padding.b],
                                [params.padding.l,
                                 params.padding.r])
                               + ([0, 0], ) * (np.ndim(in_tensor)-2),
                               mode='constant',
                               constant_values=const_pad)
            pad_w = params.padding.w
            pad_h = params.padding.h
        else:
            pad_w = pad_h = 0

        filt_w = w_dims.w
        filt_h = w_dims.h
        filt_dil_h = params.dilation.h
        filt_dil_w = params.dilation.w
        filt_str_h = params.stride.h
        filt_str_w = params.stride.w

        in_h = in_dims.h
        in_w = in_dims.w
        in_c = in_dims.c
        out_h = out_dims.h
        out_w = out_dims.w
        out_c = w_dims.out_c

        in_c_per_group = in_dims.c // params.groups
        out_c_per_group = out_c // params.groups
        in_c_off = 0
        out_c_cnt = 0

        dilated_filter_w = (filt_dil_w - 1) * (filt_w - 1) + filt_w
        dilated_filter_h = (filt_dil_h - 1) * (filt_h - 1) + filt_h

        const_h = pad_h + in_h - dilated_filter_h + 1
        const_w = pad_w + in_w - dilated_filter_w + 1

        if params.has_bias:
            if acc_q != qrec.in_qs[2]:
                biases = acc_q.expand_from(biases, qrec.in_qs[2])
            result = np.broadcast_to(biases.reshape(
                out_c, 1, 1), (out_c, out_h, out_w)).copy().astype(acc_q.dtype)
        else:
            result = np.zeros((out_c, out_h, out_w),
                              dtype=acc_q.dtype)

        calc_dtype = calc_q.dtype
        if FORCE_INT64:
            result = result.astype(np.int64)
            calc_dtype = np.int64

        out_c = out_dims.c

        calc_diff_acc_q = calc_q != acc_q
        if calc_diff_acc_q:
            weights = weights.transpose(
                w_dims.transpose_to_order(['out_c', 'h', 'w', 'in_c']))

            do_conv_with_reduction(
                details, in_tensor, weights,
                acc_q, calc_q,
                filt_w, filt_h, filt_dil_h, filt_dil_w,
                filt_str_h, filt_str_w,
                out_c, in_c_per_group, out_c_per_group, in_c_off, out_c_cnt,
                result, const_h, const_w, calc_dtype)
        else:
            weights = weights.transpose(
                w_dims.transpose_to_order(['h', 'w', 'in_c', 'out_c']))
            result, min_acc, max_acc = fast_numpy_conv(
                in_tensor,
                weights,
                result,
                in_c,
                out_h,
                out_w,
                out_c,
                dilated_filter_h,
                dilated_filter_w,
                filt_h,
                filt_w,
                filt_dil_h,
                filt_dil_w,
                filt_str_h,
                filt_str_w,
                const_h,
                const_w,
                params.groups,
                params.is_depthwise_conv(),
                calc_dtype
            )

            if details:
                details['min_acc'], details['max_acc'] = min_acc, max_acc

        result = apply_multiplicative_bias(
            qrec, params, result, 0, ktype="symmetric")

        result = result.transpose(
            out_dims.transpose_from_order(['c', 'h', 'w']))

        if qrec.out_qs[0] != acc_q:
            result = qrec.out_qs[0].reduce_from(
                result, acc_q, allow_zero_adjust=True)

        return qrec.get_outputs(params, [result], ktype="symmetric")
