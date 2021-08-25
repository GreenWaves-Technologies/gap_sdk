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
from graph.types import Conv2DParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.multiplicative.mulbias import (apply_multiplicative_bias,
                                                 apply_zero_offset_bias)
from quantization.new_qrec import QRec

FORCE_INT64 = False

# pylint: disable=invalid-name

LOG = logging.getLogger("nntool." + __name__)


@params_type(Conv2DParameters)
@qrec_type('symmetric', 'scaled', 'scaled_ne16')
class Conv2DSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        '''3D convolution by sub-matrix summing.
        '''
        details = kwargs.get('details')

        in_dims, out_dims = tuple(dims[0] for dims in cls.calc_transposed_dims(params))
        prepared_in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        # if zero offset is already applied in biases by constant quantizer this does nothing
        prepared_in_tensors = apply_zero_offset_bias(qrec, params, prepared_in_tensors, ktype="symmetric")
        in_tensor = prepared_in_tensors[0]
        # expand the weights to apply the zero offset
        weights = prepared_in_tensors[1].astype(np.int32) - qrec.in_qs[1].zero_point.astype(np.int32)
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

        weights = weights.transpose(
            params.filter.transpose_to_order(['out_c', 'h', 'w', 'in_c']))

        filt_w = params.filter.w
        filt_h = params.filter.h

        in_w = in_dims.w
        in_h = in_dims.h
        out_c = params.filter.out_c

        in_c_per_group = in_dims.c // params.groups
        out_c_per_group = out_c // params.groups
        in_c_off = 0
        out_c_cnt = 0

        dillated_filter_w = (params.dilation.w - 1) * (filt_w - 1) + filt_w
        dillated_filter_h = (params.dilation.h - 1) * (filt_h - 1) + filt_h

        out_w = ((in_w - dillated_filter_w + pad_w)) // params.stride.w + 1
        out_h = ((in_h - dillated_filter_h + pad_h)) // params.stride.h + 1

        if params.has_bias:
            # biases = qrec.prepare_biases(params, params.biases, params.weights, ktype="symmetric")
            if acc_q != qrec.in_qs[2]:
                biases = acc_q.expand_from(biases, qrec.in_qs[2])
            result = np.broadcast_to(biases.reshape(
                out_c, 1, 1), (out_c, out_h, out_w)).copy().astype(acc_q.dtype)
        else:
            result = np.zeros((out_c, out_h, out_w),
                              dtype=acc_q.dtype)

        const_h = pad_h + in_h - dillated_filter_h + 1
        const_w = pad_w + in_w - dillated_filter_w + 1
        if FORCE_INT64:
            result = result.astype(np.int64)
        for out_c_i in range(out_dims.c):
            for cur_h in range(filt_h):
                for cur_w in range(filt_w):

                    # selects all elements that the filter element needs to multiply
                    slabhw = np.multiply(in_tensor[cur_h * params.dilation.h:
                                                   const_h + cur_h * params.dilation.h:
                                                   params.stride.h,
                                                   cur_w * params.dilation.w:
                                                   const_w + cur_w * params.dilation.w:
                                                   params.stride.w,
                                                   in_c_off:
                                                   in_c_off + in_c_per_group:
                                                   1],
                                         weights[out_c_i, cur_h, cur_w],
                                         dtype=np.int64 if FORCE_INT64 else calc_q.dtype)

                    if calc_q != acc_q:
                        slabhw = acc_q.reduce_from(slabhw, calc_q)

                    # add depthwise
                    slabhw = slabhw.sum(
                        axis=-1, dtype=np.int64 if FORCE_INT64 else calc_q.dtype)
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

        result = apply_multiplicative_bias(
            qrec, params, result, 0, ktype="symmetric")

        result = result.transpose(
            out_dims.transpose_from_order(['c', 'h', 'w']))

        if qrec.out_qs[0] != acc_q:
            result = qrec.out_qs[0].reduce_from(result, acc_q, allow_zero_adjust=True)

        return qrec.get_outputs(params, [result], ktype="symmetric")
