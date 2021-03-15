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
from quantization.float32.float32_quantization import \
    Float32QuantizationRecord, Float32ScalableFilterQuantizationRecord
from quantization.kernels.kernel_base import (KernelBase, params_type,
                                              quantization)
from quantization.quantization_record_base import QuantizationRecordBase

# pylint: disable=invalid-name

LOG = logging.getLogger("nntool." + __name__)


@params_type(Conv2DParameters)
@quantization('float32')
class Conv2DFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QuantizationRecordBase,
                **kwargs):
        '''3D convolution by sub-matrix summing.
        '''
        details = kwargs.get('details')
        # qrec is set by default to Float32QuantizationRecord if None
        if qrec is None or isinstance(qrec, Float32QuantizationRecord):
            qrec = Float32ScalableFilterQuantizationRecord()
        in_dims = params.in_dims[0]
        out_dims = params.out_dims[0]
        prepared_in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
        in_tensor = prepared_in_tensors[0]
        weights = prepared_in_tensors[1]
        biases = prepared_in_tensors[2]

        if details is not None:
            details['min_acc'] = float("Infinity")
            details['max_acc'] = float("-Infinity")
            details['min_pre_mul_bias'] = float("Infinity")
            details['max_pre_mul_bias'] = float("-Infinity")

        in_tensor = in_tensor.transpose(
            in_dims.transpose_to_order(['h', 'w', 'c']))
        if params.padding.h + params.padding.w > 0:
            in_tensor = np.pad(in_tensor,
                               ([params.padding.t,
                                 params.padding.b],
                                [params.padding.l,
                                 params.padding.r])
                               + ([0, 0], ) * (np.ndim(in_tensor)-2),
                               mode='constant',
                               constant_values=0.0)
            pad_w = params.padding.w
            pad_h = params.padding.h
        else:
            pad_w = pad_h = 0

        weights = weights.transpose(params.filter.transpose_to_order(
            ['out_c', 'h', 'w', 'in_c']))

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
            # biases = qrec.prepare_biases(params, params.get_uncompressed_biases(),
            #                              params.get_uncompressed_weights(), ktype="float32")
            result = np.broadcast_to(biases.reshape(out_c, 1, 1), (out_c, out_h, out_w)).copy().astype(qrec.dtype(ktype="float32"))
        else:
            result = np.zeros((out_c, out_h, out_w),
                              dtype=qrec.dtype(ktype="float32"))

        const_h = pad_h + in_h - dillated_filter_h + 1
        const_w = pad_w + in_w - dillated_filter_w + 1
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
                                         dtype=qrec.dtype(ktype="float32"))

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

        if details is not None:
            details['min_pre_mul_bias'] = min(np.min(result), details['min_pre_mul_bias'])
            details['max_pre_mul_bias'] = max(np.max(result), details['max_pre_mul_bias'])

        result = qrec.apply_multiplicative_bias(params, result, axis=0, ktype="float32")

        result = result.transpose(out_dims.transpose_from_order(['c', 'h', 'w']))

        return qrec.get_outputs(params, [result], ktype="float32")
