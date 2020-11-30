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
from importer.tflite2.common import NoQuantizationError
from quantization.multiplicative.mult_quantization import \
    MultScalableFilterQuantizationRecord
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import \
    MultMulBiasScaleQType
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import \
    SymmetricMultBiasesQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from utils.node_id import NodeId

from .filter_pad_mixin import FilterPadMixin


class FilterMixin(FilterPadMixin):
    TF_LITE_FILT_IN_C = 0
    TF_LITE_FILT_H = 1
    TF_LITE_FILT_W = 2
    TF_LITE_FILT_OUT_C = 3
    TF_LITE_FILTER_ORDER = ['out_c', 'h', 'w', 'in_c']
    # in_c should always be 1 in this case
    # Why does TFLite use this weird order for depthwise?
    TF_LITE_DW_FILTER_ORDER = ['in_c', 'h', 'w', 'out_c']
    TF_LITE_DW_FILTER_TRANSPOSE = [3, 1, 2, 0]

    @classmethod
    def make_weights_symmetric(cls, params, input_tensors, opts):
        biases_scales = input_tensors[2].scale if params.has_bias else np.array([1], dtype=np.int32)
        # already symmetric or something we don't know
        if input_tensors[1].dtype != np.uint8:
            return input_tensors[1].scale, biases_scales, None, None
        weights_scales = input_tensors[1].scale
        # symmetric unsigned. just change zero point scale stays the same
        if np.all(input_tensors[1].zero_point == 128):
            params.weights = (params.weights.astype(np.int64) - 128).astype(np.int8)
            return weights_scales, biases_scales, None, None
        # asymmetric unsigned. change zero point and rescale
        if opts.get('rescale_perchannel'):
            return cls.scale_weights_by_channel(params, weights_scales, biases_scales,
                                                input_tensors[0].qtype.scale,
                                                zero_point=input_tensors[1].zero_point)
        else:
            return cls.scale_weights_by_tensor(params, weights_scales, biases_scales,
                                               input_tensors[0].qtype.scale,
                                               zero_point=input_tensors[1].zero_point)

    @classmethod
    def scale_weights_by_tensor(cls, params, weights_scales, biases_scales, in_scale, zero_point=None):
        if zero_point is None:
            zero_point = np.array([0])
        if params.has_bias:
            dq_biases = params.biases * biases_scales
        else:
            dq_biases = np.array([0] * params.filter.out_c, dtype=np.float32)

        if len(weights_scales) > 1:
            raise ValueError('You should not rescale perchannel weights to pertensor format')

        dq_weights = (params.weights.astype(np.float32) - zero_point) * weights_scales
        w_min = min(np.min(dq_weights), 0)
        w_max = max(np.max(dq_weights), 0)
        w_max = w_max if w_min != w_max and w_max == 0 else 1

        w_abs_max = max(w_max, np.abs(w_min))
        new_weights_scale = w_abs_max / 127
        int8_iinfo = np.iinfo(np.int8)
        int32_iinfo = np.iinfo(np.int32)
        new_biases_scale = new_weights_scale * in_scale
        params.weights = np.clip(np.floor(dq_weights / new_weights_scale + 0.5),
                                 int8_iinfo.min,
                                 int8_iinfo.max).astype(np.int8)
        params.biases = np.clip(np.floor(dq_biases / new_biases_scale + 0.5),
                                int32_iinfo.min,
                                int32_iinfo.max).astype(np.int32)
        return np.array([new_weights_scale]), np.array([new_biases_scale]),\
            np.array([w_min]), np.array([w_max])

    @classmethod
    def scale_weights_by_channel(cls, params, weights_scales, biases_scales, in_scale, zero_point=None):
        # scale weights by channel optionally correcting zero point
        if zero_point is None:
            zero_point = np.array([0])

        out_idx = params.filter.get_order_idx('out_c')
        actual_len = len(params.filter.actual_shape)
        ones_shape = tuple(params.filter.out_c if idx ==
                           out_idx else 1 for idx in range(actual_len))
        filter_axis = tuple(idx for idx in range(actual_len) if idx != out_idx)

        if params.has_bias:
            dq_biases = params.biases * biases_scales
        else:
            dq_biases = np.array([0] * params.filter.out_c, dtype=np.float32)

        if len(weights_scales) > 1:
            weights_scales = weights_scales.reshape(ones_shape)
        if len(zero_point) > 1:
            zero_point = zero_point.reshape(ones_shape)
        dq_weights = (params.weights.astype(np.float32) - zero_point) * weights_scales

        w_mins = np.minimum(np.min(dq_weights, axis=filter_axis), 0)
        w_maxes = np.maximum(np.max(dq_weights, axis=filter_axis), 0)

        w_zero_cond = np.logical_and(w_mins == w_maxes, w_maxes == 0)
        w_maxes = np.where(w_zero_cond, 1, w_maxes)

        w_abs_maxes = np.maximum(np.abs(w_mins), w_maxes)
        new_weights_scales = w_abs_maxes / 127
        int8_iinfo = np.iinfo(np.int8)
        int32_iinfo = np.iinfo(np.int32)
        new_biases_scales = new_weights_scales * in_scale
        old_err = np.seterr(all='raise')
        params.weights = np.clip(np.floor(dq_weights / new_weights_scales.reshape(ones_shape) + 0.5),
                                 int8_iinfo.min,
                                 int8_iinfo.max).astype(np.int8)
        params.biases = np.clip(np.floor(dq_biases / new_biases_scales + 0.5),
                                int32_iinfo.min,
                                int32_iinfo.max).astype(np.int32)
        np.seterr(**old_err)
        return new_weights_scales, new_biases_scales, w_mins, w_maxes

    @classmethod
    def detect_small_scales(cls, params, weights_scales, biases_scales, in_scale):
        # at this point all tensors are in expected formats
        # weights int8 biases int32 channel scaled
        tiny_weight_scales = weights_scales < SymmetricMultQType.kNearZeroTolerance
        if np.count_nonzero(tiny_weight_scales) == 0:
            return weights_scales, biases_scales

        out_idx = params.filter.get_order_idx('out_c')
        shape = tuple(slice(None) if idx !=
                      out_idx else tiny_weight_scales for idx in range(len(params.weights.shape)))

        params.weights[shape] = 0
        dq_biases = params.biases * biases_scales
        weights_scales = np.where(tiny_weight_scales, 1, weights_scales)
        biases_scales = in_scale * weights_scales
        int32_iinfo = np.iinfo(np.int32)
        params.biases = np.clip(np.floor(dq_biases / biases_scales + 0.5),
                                int32_iinfo.min,
                                int32_iinfo.max).astype(np.int32)
        return weights_scales, biases_scales

    @classmethod
    def fix_weights_and_biases(cls, params, input_tensors, opts):
        weights_scales, biases_scales, w_mins, w_maxes = cls.make_weights_symmetric(
            params, input_tensors, opts)
        if opts.get('rescale_perchannel'):
            if len(weights_scales) != params.filter.out_c:
                weights_scales, biases_scales, w_mins, w_maxes = cls.scale_weights_by_channel(
                    params, weights_scales, biases_scales, input_tensors[0].qtype.scale)
            weights_scales, biases_scales = cls.detect_small_scales(
                params, weights_scales, biases_scales, input_tensors[0].scale)
        if w_mins is None:
            w_mins = input_tensors[1].min_val
            w_maxes = input_tensors[1].max_val
        if isinstance(w_mins, int):
            w_mins = np.asarray([w_mins] * params.filter.out_c)
        if isinstance(w_maxes, int):
            w_maxes = np.asarray([w_maxes] * params.filter.out_c)
        return weights_scales, biases_scales, w_mins, w_maxes

    @classmethod
    def load_filter_parameters(cls, G, params, input_tensors, output_tensors, opts, converted_to_conv=False):
        if opts.get('load_tensors') or opts.get('load_quantization'):
            params.weights = input_tensors[1].value
            if converted_to_conv:
                params.weights = params.weights.transpose(cls.TF_LITE_DW_FILTER_TRANSPOSE)
            if params.has_bias:
                params.biases = input_tensors[2].value

        if opts.get('load_quantization'):
            if input_tensors[0].qtype is None:
                raise NoQuantizationError("quantization not present in tflite file")
            weights_scales, biases_scales, w_mins, w_maxes = cls.fix_weights_and_biases(
                params, input_tensors, opts)
            biases_q = SymmetricMultBiasesQType(dtype=np.int32, scale=biases_scales.flatten())
            weights_q = SymmetricMultQType(
                dtype=np.int8, narrow_range=True, scale=weights_scales.flatten(), min_val=w_mins, max_val=w_maxes)
            in_q = input_tensors[0].qtype
            out_q = output_tensors[0].qtype
            mulbiases_q = MultMulBiasScaleQType.from_filter(in_q, weights_q, out_q, params)
            qrec = MultScalableFilterQuantizationRecord(in_qs=[in_q],
                                                        out_qs=[out_q],
                                                        mul_biases_q=mulbiases_q,
                                                        weights_q=weights_q,
                                                        biases_q=biases_q)
            G.quantization[NodeId(params)] = qrec

    @classmethod
    def load_dequantized_filter_parameters(cls, params, input_tensors, converted_to_conv=False, is_dw=False):
        weights_scales = input_tensors[1].scale
        in_scale = input_tensors[0].scale
        weights_quant = input_tensors[1].value
        # save in the node the dequantized values
        if len(weights_scales) > 1:  # tf2 conv and dw (fully connected should be per-tensor)
            if is_dw:
                # depthwise
                shape_pc = tuple(size if idx == 3 else 1  # always along axis 3 from tflite quantization spec
                                 for idx, size in enumerate(weights_quant.shape))
            else:
                # normal convolution
                shape_pc = tuple(size if idx == 0 else 1  # always along axis 0 from tflite quantization spec
                                 for idx, size in enumerate(weights_quant.shape))
            params.weights = (weights_quant.astype(np.int64) - input_tensors[1].zero_point.reshape(shape_pc)) \
                * weights_scales.reshape(shape_pc)
        else:
            params.weights = (weights_quant - input_tensors[1].zero_point) * weights_scales
        if converted_to_conv:
            params.weights = params.weights.transpose(cls.TF_LITE_DW_FILTER_TRANSPOSE)
        if params.has_bias:
            biases_scales = weights_scales * in_scale
            params.biases = input_tensors[2].value * biases_scales
