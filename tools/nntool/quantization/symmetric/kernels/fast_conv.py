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

from quantization.quantization_record_base import \
    ScalableFilterQuantizationRecordBase

FORCE_INT64 = False

# pylint: disable=invalid-name

LOG = logging.getLogger("nntool." + __name__)

def faster_conv(params,
                in_tensors,
                qrec: ScalableFilterQuantizationRecordBase,
                details=None):
    '''3D convolution by sub-matrix summing.
    '''
    in_dims = params.in_dims[0]
    out_dims = params.out_dims[0]
    weights = qrec.prepare_weights(params, params.weights, ktype="symmetric")
    in_tensor = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")[0]

    if details is not None:
        details['min_acc'] = float("Infinity")
        details['max_acc'] = float("-Infinity")

    in_tensor = in_tensor.transpose(in_dims.transpose_to_order(['h', 'w', 'c']))
    if params.padding.h + params.padding.w > 0:
        if hasattr(qrec.in_qs[0], 'zero_point'):
            const_pad = qrec.in_qs[0].zero_point[0]
        else:
            const_pad = 0
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


    dillated_filter_w = filt_w if params.dilation.w == 1 else filt_w * params.dilation.w - 1
    dillated_filter_h = filt_h if params.dilation.h == 1 else filt_w * params.dilation.h - 1

    out_w = ((in_w - dillated_filter_w + pad_w)) // params.stride.w + 1
    out_h = ((in_h - dillated_filter_h + pad_h)) // params.stride.h + 1

    if params.has_bias:
        biases = qrec.prepare_biases(params, params.biases, params.weights, ktype="symmetric")
        if qrec.acc_q != qrec.biases_q:
            biases = qrec.acc_q.expand_from(biases, qrec.biases_q)
        result = np.ones((out_c, out_h, out_w),
                         dtype=qrec.acc_q.dtype) * biases.reshape(out_c, 1, 1)
    else:
        result = np.zeros((out_c, out_h, out_w),
                          dtype=qrec.acc_q.dtype)

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
                                     dtype=np.int64 if FORCE_INT64 else qrec.calc_q.dtype)

                if qrec.calc_q != qrec.acc_q:
                    slabhw = qrec.acc_q.reduce_from(slabhw, qrec.calc_q)

                # add depthwise
                slabhw = slabhw.sum(axis=-1, dtype=np.int64 if FORCE_INT64 else qrec.calc_q.dtype)
                # add to the previous filter elements
                result[out_c_i] += slabhw

                if details is not None:
                    details['min_acc'] = min(np.min(result[out_c_i]), details['min_acc'])
                    details['max_acc'] = max(np.max(result[out_c_i]), details['max_acc'])

        out_c_cnt += 1
        if out_c_cnt >= out_c_per_group:
            out_c_cnt = 0
            in_c_off += in_c_per_group

    result = qrec.apply_multiplicative_bias(params, result, 0, ktype="symmetric")

    result = result.transpose(out_dims.transpose_from_order(['c', 'h', 'w']))

    if qrec.out_qs[0] != qrec.acc_q:
        result = qrec.out_qs[0].reduce_from(result, qrec.acc_q)

    return qrec.get_outputs(params, [result], ktype="symmetric")
