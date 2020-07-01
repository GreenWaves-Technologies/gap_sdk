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
from typing import Sequence, Tuple, Mapping

import numpy as np

from graph.types import (FilterParameters, MultiplicativeBiasParameters,
                         Parameters, ConstantInputParameters)
from quantization.multiplicative.mult_qtype_base import (MultQTypeBase,
                                                         WrapperMixin)
from quantization.multiplicative.symmetric.mult_mulbias_qtype_new import (
    MultMulBiasQType, MultMulBiasScaleQType)
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import \
    SymmetricMultBiasesQType
from quantization.qtype import QType
from quantization.quantization_record_base import (
    ConstantQuantizationRecordBase, InputOutputQuantizationRecordBase,
    QuantizationRecordBase, ScalableFilterQuantizationRecordBase, HasConstantsBase)


class MultQuantizationRecordBase(QuantizationRecordBase):
    def __init__(self, *args, quant_mode="symmetric", info=None, **kwargs):
        super(MultQuantizationRecordBase, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['quant_mode'] = quant_mode
        self._cache = {}

    def clear_cache(self):
        self._cache = {}

    def check_cache(self, name):
        return self._cache.get(name)

    def dequantize_as(self, tensor: np.ndarray, key_name: str, idx: int = None) -> np.ndarray:
        qtype = getattr(self, key_name)
        if idx is not None:
            qtype = qtype[idx]
        return qtype.dequantize(tensor)

    def quantize_as(self, tensor: np.ndarray, key_name: str, idx: int = None) -> np.ndarray:
        qtype = getattr(self, key_name)
        if idx is not None:
            qtype = qtype[idx]
        return qtype.quantize(tensor)

    def dequantize_wrapped(self, tensor: np.ndarray, key_name: str, idx: int = None) -> np.ndarray:
        qtype = getattr(self, key_name)
        if idx is not None:
            qtype = qtype[idx]
        if isinstance(qtype, WrapperMixin):
            return qtype.wrapped.dequantize(tensor)
        return qtype.dequantize(tensor)

    def quantize_wrapped(self, tensor: np.ndarray, key_name: str, idx: int = None) -> np.ndarray:
        qtype = getattr(self, key_name)
        if idx is not None:
            qtype = qtype[idx]
        if isinstance(qtype, WrapperMixin):
            return qtype.wrapped.quantize(tensor)
        return qtype.quantize(tensor)

    def requantize(self, tensor: np.ndarray, key_name: str, idx: int = None) -> np.ndarray:
        qtype = getattr(self, key_name)
        if idx is not None:
            qtype = qtype[idx]

        if isinstance(qtype, WrapperMixin):
            tensor_fp = self.dequantize_wrapped(tensor, key_name, idx=idx)
            tensor_sym = qtype.quantize(tensor_fp)
            return tensor_sym
        if tensor.dtype == np.float32:
            return qtype.quantize(tensor)
        return tensor

    def confirm_dimension(self, out_c_idx: int, key_name: str):
        qtype = getattr(self, key_name)
        qtype.quantized_dimension = out_c_idx


class InputQuantizationMixin(MultQuantizationRecordBase):
    def __init__(self, *args, auto_quantize_inputs=False, auto_dequantize_inputs=False, **kwargs):
        super(InputQuantizationMixin, self).__init__(*args, **kwargs)
        self._auto_quantize_inputs = auto_quantize_inputs
        self._auto_dequantize_inputs = auto_dequantize_inputs

    @property
    def auto_quantize_inputs(self):
        return self._auto_quantize_inputs

    @auto_quantize_inputs.setter
    def auto_quantize_inputs(self, val):
        self._auto_quantize_inputs = val

    @property
    def auto_dequantize_inputs(self):
        return self._auto_dequantize_inputs

    @auto_quantize_inputs.setter
    def auto_quantize_inputs(self, val):
        self._auto_dequantize_inputs = val

    def prepare_inputs(self, params: Parameters,
                       input_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        del params
        if ktype == 'float32' and self.auto_dequantize_inputs:
            return [self.dequantize_wrapped(input_tensor, "in_qs", idx=idx)
                    for idx, input_tensor in enumerate(input_tensors)]
        if ktype == 'symmetric' and self.auto_quantize_inputs:
            return [self.quantize_as(input_tensor, "in_qs", idx=idx) for idx, input_tensor in enumerate(input_tensors)]
        return input_tensors


class OutputQuantizationMixin(MultQuantizationRecordBase):
    def __init__(self, *args, auto_dequantize_outputs=False, auto_quantize_outputs=False, **kwargs):
        super(OutputQuantizationMixin, self).__init__(*args, **kwargs)
        self._auto_quantize_outputs = auto_quantize_outputs
        self._auto_dequantize_outputs = auto_dequantize_outputs

    @property
    def auto_dequantize_outputs(self):
        return self._auto_dequantize_outputs

    @auto_dequantize_outputs.setter
    def auto_dequantize_outputs(self, val):
        self._auto_dequantize_outputs = val

    @property
    def auto_quantize_outputs(self):
        return self._auto_dequantize_outputs

    @auto_quantize_outputs.setter
    def auto_quantize_outputs(self, val):
        self._auto_dequantize_outputs = val

    def get_outputs(self, params: Parameters,
                    output_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        del params
        if ktype == 'symmetric':
            if self._auto_dequantize_outputs:
                return [self.dequantize_as(output_tensor, "out_qs", idx=idx)
                        for idx, output_tensor in enumerate(output_tensors)]
            output_tensors = [self.out_qs[idx].clip(output_tensor)
                              for idx, output_tensor in enumerate(output_tensors)]
        return output_tensors


class MultQuantizationRecord(InputQuantizationMixin, OutputQuantizationMixin, InputOutputQuantizationRecordBase):
    def __init__(self, *args, scale_mul_biases_q=None, info=None, **kwargs):
        super(MultQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self.scale_mul_biases_q = scale_mul_biases_q

    @property
    def scale_mul_biases_q(self):
        mul_biases_q = self._info.get('scale_mul_biases_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_mul_biases_q = mul_biases_q
        return mul_biases_q

    @scale_mul_biases_q.setter
    def scale_mul_biases_q(self, val):
        self._info['scale_mul_biases_q'] = val

    def set_scale(self, in_idx=0, out_idx=0, extra_scale=1):
        if isinstance(in_idx, int):
            in_scale = self.in_qs[in_idx].scale
        else:
            in_scale = reduce(lambda x, y: x * y, [self.in_qs[idx].scale for idx in in_idx])
        if isinstance(out_idx, int):
            out_scale = self.out_qs[out_idx].scale
        else:
            out_scale = reduce(lambda x, y: x * y, [self.out_qs[idx].scale for idx in out_idx])
        scale_mul_biases_q = self.scale_mul_biases_q
        scale = in_scale * extra_scale / out_scale
        scale_mul_biases_q.scale = scale


class MultAddQuantizationRecord(MultQuantizationRecord):
    def __init__(self, *args, scale_in_mul_biases_q=None, info=None, **kwargs):
        super(MultAddQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['scale_in_mul_biases_q'] = scale_in_mul_biases_q

    @property
    def scale_in_mul_biases_q(self):
        mul_biases_q = self._info.get('scale_in_mul_biases_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_in_mul_biases_q = mul_biases_q
        return mul_biases_q

    @scale_in_mul_biases_q.setter
    def scale_in_mul_biases_q(self, val):
        self._info['scale_in_mul_biases_q'] = val

    @property
    def scaled_idx(self):
        return 1 if self.in_qs[1].scale > self.in_qs[0].scale else 0

    def set_add_scale(self):
        self.set_scale(in_idx=0 if self.scaled_idx else 1)
        scale_in_mul_biases_q = self.scale_in_mul_biases_q
        scaled_idx = self.scaled_idx
        not_scaled_idx = 0 if scaled_idx else 1
        scale = self.in_qs[scaled_idx].scale / self.in_qs[not_scaled_idx].scale
        scale_in_mul_biases_q.scale = scale


class MultConstantQuantizationRecord(InputQuantizationMixin, InputOutputQuantizationRecordBase,
                                     OutputQuantizationMixin, ConstantQuantizationRecordBase):
    def gen_value(self, value):
        return self.out_qs[0].get_quantized(value, container_is_quantized=self.constants_are_quantized)


class FilterQuantizationMixin(MultQuantizationRecord):
    @property
    def calc_q(self) -> MultQTypeBase:
        return QType(bits=32, q=0, signed=True)

    @property
    def acc_q(self) -> MultQTypeBase:
        return QType(bits=32, q=0, signed=True)

    @property
    def biases_q(self) -> SymmetricMultBiasesQType:
        return self._info.get('biases_q')

    @property
    def weights_q(self) -> MultQTypeBase:
        return self._info.get('weights_q')

    @calc_q.setter
    def calc_q(self, val: MultQTypeBase):
        pass

    @acc_q.setter
    def acc_q(self, val: MultQTypeBase):
        pass

    @biases_q.setter
    def biases_q(self, val: SymmetricMultBiasesQType):
        self._info['biases_q'] = val

    @weights_q.setter
    def weights_q(self, val: MultQTypeBase):
        self._info['weights_q'] = val

    @staticmethod
    def rescale(arr, from_scale, to_scale):
        return np.floor((arr * from_scale/to_scale) + 0.5).astype(arr.dtype)


class MultScalableFilterQuantizationRecord(FilterQuantizationMixin, ScalableFilterQuantizationRecordBase):
    def __init__(self, *args,
                 weights_q: MultQTypeBase = None,
                 biases_q: SymmetricMultBiasesQType = None,
                 mul_biases_q: Sequence[MultMulBiasQType] = None,
                 calc_q: QType = None,
                 acc_q: QType = None,
                 enable_prenorm=False,
                 info=None,
                 **kwargs):
        super(MultScalableFilterQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['calc_q'] = calc_q
            self._info['acc_q'] = acc_q
            self._info['biases_q'] = biases_q
            self._info['weights_q'] = weights_q
            self._info['mul_biases_q'] = mul_biases_q
            self._info['enable_prenorm'] = enable_prenorm
        self.biases_q.link(self.weights_q, self.in_qs[0])

    @property
    def unwrap(self):
        return self._unwrap

    @unwrap.setter
    def unwrap(self, val):
        self._unwrap = val
        self.biases_q.link(self.weights_q, self.in_qs[0])

    def compute_prenorm(self, params: FilterParameters):
        if not self.enable_prenorm:
            return 0
        max_bits = self.in_qs[0].bits - 1 + self.weights_q.bits - 1 + 1 + \
            math.ceil(math.log2(params.filter.in_c * params.filter.h * params.filter.w))
        spare_bits = 31 - max_bits
        if self.mul_biases_q.dtype == np.int8:
            bits = 7
        elif self.mul_biases_q.dtype == np.uint8:
            bits = 8
        return max(0, bits - spare_bits)

    @property
    def enable_prenorm(self) -> bool:
        return self._info.get('enable_prenorm')

    @enable_prenorm.setter
    def enable_prenorm(self, val: bool):
        self._info['enable_prenorm'] = val

    @property
    def mul_biases_q(self) -> MultMulBiasQType:
        return self._info.get('mul_biases_q')

    @mul_biases_q.setter
    def mul_biases_q(self, val: MultMulBiasQType):
        self._info['mul_biases_q'] = val

    def get_quantized_bias_offset(self, params, weights):
        # input zero correction is sum(W * Zin) by out_c if weights are channel scaled
        axis = tuple([idx for idx in range(4) if idx != params.filter.get_order_idx('out_c')])
        return np.sum(np.multiply(self.in_qs[0].zero_point,
                                  weights,
                                  dtype=np.int32),
                      dtype=np.int32,
                      axis=axis)

    @property
    def biases_zero_correction(self):
        # output zero correction is So/(Si * Sw) * ZPo by out_c if weights are channel scaled
        scale = self.out_qs[0].scale / (self.in_qs[0].scale * self.weights_q.scale)
        return np.floor((self.out_qs[0].zero_point * scale) + 0.5).astype(np.int32)

    def prepare_biases(self, params: Parameters, biases: np.ndarray,
                       weights: np.ndarray, ktype: str = None) -> np.ndarray:
        if ktype == 'float32':
            return self.biases_q.get_dequantized(biases,
                                                 container_is_quantized=self.constants_are_quantized).astype(np.float32)
        if ktype == 'symmetric':
            return self.gen_biases(params, biases, weights)
        raise ValueError()

    def prepare_weights(self, params: Parameters,
                        weights: np.ndarray, ktype: str = None) -> np.ndarray:
        self.confirm_dimension(params.filter.get_order_idx('out_c'), 'weights_q')
        if ktype == 'float32':
            weights = self.weights_q.get_dequantized(weights,
                                                     container_is_quantized=self.constants_are_quantized)
            return weights.astype(np.float32)
        if ktype == 'symmetric':
            return self.gen_weights(params, weights)
        raise ValueError()

    def gen_weights(self, params: Parameters, weights: np.ndarray) -> np.ndarray:
        return self.weights_q.get_quantized(weights,
                                            container_is_quantized=self.constants_are_quantized)

    def gen_biases(self, params: Parameters, biases: np.ndarray, weights: np.ndarray) -> np.ndarray:
        biases = self.biases_q.get_quantized(
            biases, container_is_quantized=self.constants_are_quantized)
        if self.in_qs[0].zero_point != 0:
            biases -= self.get_quantized_bias_offset(params,
                                                     self.requantize(weights, 'weights_q'))
        if self.out_qs[0].zero_point != 0:
            biases += self.biases_zero_correction
        return biases

    def gen_mul_biases(self, params: MultiplicativeBiasParameters) -> np.ndarray:
        if isinstance(self.mul_biases_q, MultMulBiasQType):
            self.mul_biases_q.pre_normalization = self.compute_prenorm(params)
        return self.mul_biases_q.qbiases

    def apply_multiplicative_bias(self, params: FilterParameters, input_tensor: np.ndarray,
                                  axis: int, ktype: str = None):
        if ktype == 'float32':
            return input_tensor
        if ktype == 'symmetric':
            if isinstance(self.mul_biases_q, MultMulBiasQType):
                self.mul_biases_q.pre_normalization = self.compute_prenorm(params)
            input_tensor = self.mul_biases_q.apply_scales(input_tensor, axis)
            return input_tensor.astype(np.int32)

class MultSSDDetectorQuantizationRecord(MultQuantizationRecord):
    def __init__(self, *args, scale_x_q=None, scale_x_anc_q=None, scale_y_q=None,
                 scale_y_anc_q=None, scale_h_q=None, scale_w_q=None, scale_ao_q=None,
                 info=None, **kwargs):
        super(MultSSDDetectorQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['scale_x_q'] = scale_x_q
            self._info['scale_y_q'] = scale_y_q
            self._info['scale_x_anc_q'] = scale_x_anc_q
            self._info['scale_y_anc_q'] = scale_y_anc_q
            self._info['scale_h_q'] = scale_h_q
            self._info['scale_w_q'] = scale_w_q
            self._info['scale_ao_q'] = scale_ao_q

    @property
    def scale_x_q(self):
        mul_biases_q = self._info.get('scale_x_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_x_q = mul_biases_q
        return mul_biases_q

    @scale_x_q.setter
    def scale_x_q(self, val):
        self._info['scale_x_q'] = val

    @property
    def scale_x_anc_q(self):
        mul_biases_q = self._info.get('scale_x_anc_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_x_anc_q = mul_biases_q
        return mul_biases_q

    @scale_x_anc_q.setter
    def scale_x_anc_q(self, val):
        self._info['scale_x_anc_q'] = val

    @property
    def scale_y_q(self):
        mul_biases_q = self._info.get('scale_y_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_y_q = mul_biases_q
        return mul_biases_q

    @scale_y_q.setter
    def scale_y_q(self, val):
        self._info['scale_y_q'] = val

    @property
    def scale_y_anc_q(self):
        mul_biases_q = self._info.get('scale_y_anc_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_y_anc_q = mul_biases_q
        return mul_biases_q

    @scale_y_anc_q.setter
    def scale_y_anc_q(self, val):
        self._info['scale_y_anc_q'] = val

    @property
    def scale_h_q(self):
        mul_biases_q = self._info.get('scale_h_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_h_q = mul_biases_q
        return mul_biases_q

    @scale_h_q.setter
    def scale_h_q(self, val):
        self._info['scale_h_q'] = val

    @property
    def scale_w_q(self):
        mul_biases_q = self._info.get('scale_w_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_w_q = mul_biases_q
        return mul_biases_q

    @scale_w_q.setter
    def scale_w_q(self, val):
        self._info['scale_w_q'] = val

    @property
    def scale_ao_q(self):
        mul_biases_q = self._info.get('scale_ao_q')
        if mul_biases_q is None:
            mul_biases_q = MultMulBiasScaleQType(dtype=np.uint8)
            self.scale_ao_q = mul_biases_q
        return mul_biases_q

    @scale_ao_q.setter
    def scale_ao_q(self, val):
        self._info['scale_ao_q'] = val

    def set_scales(self, params):
        offset_q = self.in_qs[0]
        anchors_q = self.in_qs[2]
        out_boxes_q = self.out_qs[0]
        self.scale_x_q.scale = (offset_q.scale * anchors_q.scale) / (out_boxes_q.scale * params.x_scale)
        self.scale_x_anc_q.scale = params.x_scale / offset_q.scale
        self.scale_y_q.scale = (offset_q.scale * anchors_q.scale) / (out_boxes_q.scale * params.y_scale)
        self.scale_y_anc_q.scale = params.y_scale / offset_q.scale
        self.scale_h_q.scale = offset_q.scale / params.h_scale
        self.scale_w_q.scale = offset_q.scale / params.w_scale
        self.scale_ao_q.scale = anchors_q.scale * 2**(-15) / out_boxes_q.scale

class MultScalableRnnQuantizationRecord(InputQuantizationMixin, OutputQuantizationMixin,
                                        HasConstantsBase, InputOutputQuantizationRecordBase):
    def __init__(self, *args,
                 i_2_a_q: MultMulBiasQType = None,
                 s_2_o_q: MultMulBiasQType = None,
                 s_2_s_q: MultMulBiasQType = None,
                 info=None,
                 **kwargs):
        super(MultScalableRnnQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            # scale applied to input after weights
            self._info['i_2_a_q'] = i_2_a_q
            # scale applied to state to give output
            self._info['s_2_o_q'] = s_2_o_q
            # scale applied to state to give recurrent state
            self._info['s_2_s_q'] = s_2_s_q

    @property
    def i_2_a_q(self) -> MultMulBiasQType:
        return self._info.get('i_2_a_q')

    @i_2_a_q.setter
    def i_2_a_q(self, val: MultMulBiasQType):
        self._info['i_2_a_q'] = val

    @property
    def s_2_o_q(self) -> MultMulBiasQType:
        return self._info.get('s_2_o_q')

    @s_2_o_q.setter
    def s_2_o_q(self, val: MultMulBiasQType):
        self._info['s_2_o_q'] = val

    @property
    def s_2_s_q(self) -> MultMulBiasQType:
        return self._info.get('s_2_s_q')

    @s_2_s_q.setter
    def s_2_s_q(self, val: MultMulBiasQType):
        self._info['s_2_s_q'] = val

    def scale_input(self,
                    params: Parameters,
                    weighted_input_tensor: np.ndarray,
                    axis: int,
                    consts_and_qtypes: Mapping[str,
                                               Tuple[np.ndarray,
                                                     MultMulBiasQType]],
                    ktype: str = None):
        del params, consts_and_qtypes
        if ktype != "symmetric" or self.i_2_a_q is None:
            return weighted_input_tensor
        # For AT model creation this should not be set. This is just for simulation
        # i.e. input scale == state scale == output scale
        # scale input_scale * input_weights to state_scale * recurrent_weights_scale
        weighted_input_tensor = weighted_input_tensor.astype(np.int32)
        return self.i_2_a_q.apply_scales(weighted_input_tensor, axis)

    def scale_output(self,
                     params: Parameters,
                     state_tensor: np.ndarray,
                     axis: int,
                     consts_and_qtypes: Mapping[str,
                                                Tuple[np.ndarray,
                                                      MultMulBiasQType]],
                     ktype: str = None):
        if ktype != "symmetric" or self.s_2_o_q is None:
            return state_tensor
        o_q = self.out_qs[0]
        # scale state_scale to output_scale
        return o_q.clip(self.s_2_o_q.apply_scales(state_tensor, axis))

    def scale_state(self,
                    params: Parameters,
                    state_tensor: np.ndarray,
                    axis: int,
                    consts_and_qtypes: Mapping[str,
                                               Tuple[np.ndarray,
                                                     MultMulBiasQType]],
                    ktype: str = None):
        if ktype != "symmetric" or self.s_2_s_q is None:
            return state_tensor
        # scale state_scale * recurrent_weights_scale to output_scale
        i_state_q = consts_and_qtypes['i_state'][1]
        return i_state_q.clip(self.s_2_s_q.apply_scales(state_tensor, axis))


class MultScalableLstmQuantizationRecord(InputQuantizationMixin, OutputQuantizationMixin,
                                         HasConstantsBase, InputOutputQuantizationRecordBase):
    def __init__(self, *args,
                 i_2_o_q: MultMulBiasQType = None,
                 i_2_f_q: MultMulBiasQType = None,
                 i_2_c_q: MultMulBiasQType = None,
                 i_2_i_q: MultMulBiasQType = None,
                 r_2_o_q: MultMulBiasQType = None,
                 r_2_f_q: MultMulBiasQType = None,
                 r_2_c_q: MultMulBiasQType = None,
                 r_2_i_q: MultMulBiasQType = None,
                 cell_in_q: MultMulBiasQType = None,
                 cell_out_q: MultMulBiasQType = None,
                 state_out_q: MultMulBiasQType = None,
                 i_qtype: QType = None,
                 info=None,
                 **kwargs):

        super(MultScalableLstmQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            # scale applied to input after weights
            self._info['i_2_o_q'] = i_2_o_q
            self._info['i_2_f_q'] = i_2_f_q
            self._info['i_2_c_q'] = i_2_c_q
            self._info['i_2_i_q'] = i_2_i_q
            # scale applied to input + state after weights
            self._info['r_2_o_q'] = r_2_o_q
            self._info['r_2_f_q'] = r_2_f_q
            self._info['r_2_c_q'] = r_2_c_q
            self._info['r_2_i_q'] = r_2_i_q
            # Internal qtype * 1
            self._info['i_qtype'] = i_qtype
            # Cell scales
            self._info['cell_in_q'] = cell_in_q
            self._info['cell_out_q'] = cell_out_q
            # state out scale
            self._info['state_out_q'] = state_out_q
        self.external_type = QType(bits=8, q=0, signed=True)

    def __getattr__(self, attr):
        if attr in self._info:
            return self._info[attr]
        raise AttributeError()

    def scale_to(self,
                 var,
                 tensor: np.ndarray,
                 axis: int,
                 ktype: str = None):
        if ktype != "symmetric" or getattr(self, var) is None:
            return tensor
        qtype = getattr(self, var)
        return qtype.apply_scales(tensor, axis)

    @property
    def internal_qtype(self):
        return self._info['i_qtype']

    def scale_input_input(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('i_2_i_q', tensor, axis, ktype)

    def scale_input_forget(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('i_2_f_q', tensor, axis, ktype)

    def scale_input_cell(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('i_2_c_q', tensor, axis, ktype)

    def scale_input_output(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('i_2_o_q', tensor, axis, ktype)

    def scale_internal_input(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('r_2_i_q', tensor, axis, ktype)

    def scale_internal_forget(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('r_2_f_q', tensor, axis, ktype)

    def scale_internal_cell(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('r_2_c_q', tensor, axis, ktype)

    def scale_internal_output(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('r_2_o_q', tensor, axis, ktype)

    def scale_cellin(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.scale_to('cell_in_q', tensor, axis, ktype)

    def scale_cellout(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.external_type.clip(self.scale_to('cell_out_q', tensor, axis, ktype))

    def scale_output(self, tensor: np.ndarray, axis: int, ktype: str = None):
        return self.external_type.clip(self.scale_to('state_out_q', tensor, axis, ktype))
