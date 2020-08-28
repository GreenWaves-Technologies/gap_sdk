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

from typing import Sequence

import numpy as np

from graph.types import (Conv2DParameters, MatrixAddParameters,
                         MatrixDivParameters, MatrixMulParameters,
                         MatrixSubParameters, MultiplicativeBiasParameters,
                         Parameters, GlobalPoolParameters)
from quantization.qtype import QType
from quantization.quantization_record_base import (
    FilterQuantizationRecordBase, InputOutputQuantizationRecordBase,
    ScalableFilterQuantizationRecordBase)
from utils.at_norm import at_norm


class SymmetricQuantizationBase(InputOutputQuantizationRecordBase):
    def __init__(self, *args, auto_quantize_inputs=False, auto_dequantize_outputs=False, **kwargs):
        super(SymmetricQuantizationBase, self).__init__(*args, **kwargs)
        self._auto_quantize_inputs = auto_quantize_inputs
        self._auto_dequantize_outputs = auto_dequantize_outputs

    def dequantize_as(self, tensor: np.ndarray, key_name: str, idx: int = None) -> np.ndarray:
        qtype = self._info[key_name]
        if idx:
            qtype = qtype[idx]
        return qtype.dequantize(tensor)

    def quantize_as(self, tensor: np.ndarray, key_name: str, idx: int = None) -> np.ndarray:
        qtype = self._info[key_name]
        if idx:
            qtype = qtype[idx]
        return qtype.quantize(tensor)

    @property
    def auto_quantize_inputs(self):
        return self._auto_quantize_inputs

    @auto_quantize_inputs.setter
    def auto_quantize_inputs(self, val):
        self._auto_quantize_inputs = val

    @property
    def auto_dequantize_outputs(self):
        return self._auto_dequantize_outputs

    @auto_dequantize_outputs.setter
    def auto_dequantize_outputs(self, val):
        self._auto_dequantize_outputs = val

    def prepare_inputs(self, params: Parameters,
                       input_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        del params
        if ktype == "symmetric" and self._auto_quantize_inputs:
            return [self.in_qs[idx].quantize(input_tensor) for idx, input_tensor in enumerate(input_tensors)]
        return input_tensors

    def get_outputs(self, params: Parameters,
                    output_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        if ktype == "symmetric":
            if isinstance(params, (MatrixAddParameters, MatrixSubParameters)):
                q_calc = QType(bits=32, q=min(self.in_qs[0].q, self.in_qs[1].q), signed=True)
                output_tensors = [self.out_qs[0].reduce_from(output_tensors[0], q_calc)]
            elif isinstance(params, (MatrixMulParameters, MatrixDivParameters)):
                q_calc = QType(bits=32, q=self.in_qs[0].q+self.in_qs[1].q, signed=True)
                output_tensors = [self.out_qs[0].reduce_from(output_tensors[0], q_calc)]
            elif isinstance(params, GlobalPoolParameters) and params.pool_type == "sum":
                output_tensors = [self.out_qs[0].reduce_from(output_tensors[0], self.in_qs[0])]
            if self._auto_dequantize_outputs:
                return [self.out_qs[idx].dequantize(output_tensor) for idx, output_tensor in enumerate(output_tensors)]
        return output_tensors


class SymmetricQuantizationRecord(SymmetricQuantizationBase):
    pass


class FilterSymmetricQuantizationBase(SymmetricQuantizationBase):
    @property
    def calc_q(self) -> QType:
        return self._info.get('calc_q')

    @property
    def acc_q(self) -> QType:
        return self._info.get('acc_q')

    @property
    def biases_q(self) -> QType:
        return self._info.get('biases_q')

    @property
    def weights_q(self) -> QType:
        return self._info.get('weights_q')

    @calc_q.setter
    def calc_q(self, val: QType):
        self._info['calc_q'] = val

    @acc_q.setter
    def acc_q(self, val: QType):
        self._info['acc_q'] = val

    @biases_q.setter
    def biases_q(self, val: QType):
        self._info['biases_q'] = val

    @weights_q.setter
    def weights_q(self, val: QType):
        self._info['weights_q'] = val

    def gen_weights(self, params, weights: np.ndarray) -> np.ndarray:
        return self.quantize_as(weights, 'weights_q')

    def gen_biases(self, params: Parameters, biases: np.ndarray, weights: np.ndarray) -> np.ndarray:
        del params, weights
        return self.quantize_as(biases, 'biases_q')

    def prepare_weights(self, params: Parameters, weights: np.ndarray, ktype: str = None) -> np.ndarray:
        if ktype == "symmetric":
            return self.gen_weights(params, weights)
        if ktype == "float32":
            return weights
        raise NotImplementedError()

    def prepare_biases(self, params: Parameters, biases: np.ndarray,
                       weights: np.ndarray, ktype: str = None) -> np.ndarray:
        if ktype == "symmetric":
            return self.gen_biases(params, biases, weights)
        if ktype == "float32":
            return biases
        raise NotImplementedError()


class SymmetricFilterQuantizationRecord(FilterSymmetricQuantizationBase, FilterQuantizationRecordBase):
    def __init__(self, *args,
                 weights_q: QType = None,
                 biases_q: QType = None,
                 calc_q: QType = None,
                 acc_q: QType = None,
                 info=None,
                 **kwargs):
        super(SymmetricFilterQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['calc_q'] = calc_q
            self._info['acc_q'] = acc_q
            self._info['biases_q'] = biases_q
            self._info['weights_q'] = weights_q
            self._info['weights_q'] = weights_q


class SymmetricScalableFilterQuantizationRecord(FilterSymmetricQuantizationBase, ScalableFilterQuantizationRecordBase):
    def __init__(self, *args,
                 weights_q: QType = None,
                 biases_q: QType = None,
                 mul_biases_q: QType = None,
                 calc_q: QType = None,
                 acc_q: QType = None,
                 info=None,
                 **kwargs):
        super(SymmetricScalableFilterQuantizationRecord, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['calc_q'] = calc_q
            self._info['acc_q'] = acc_q
            self._info['biases_q'] = biases_q
            self._info['weights_q'] = weights_q
            self._info['mul_biases_q'] = mul_biases_q
            self._info['weights_q'] = weights_q

    @property
    def mul_biases_q(self) -> QType:
        return self._info.get('mul_biases_q')

    @mul_biases_q.setter
    def mul_biases_q(self, val: QType):
        self._info['mul_biases_q'] = val

    def gen_mul_biases(self, params: MultiplicativeBiasParameters) -> np.ndarray:
        if params.has_mul_bias:
            return self.quantize_as(params.mul_biases, 'mul_biases_q')
        return None

    def apply_multiplicative_bias(self, params: Conv2DParameters, input_tensor: np.ndarray,
                                  axis: int, ktype: str = None):
        if ktype == "symmetric":
            if params.has_mul_bias:
                mul_biases = self.quantize_as(params.mul_biases, 'mul_biases_q')
                shape = [params.filter.out_c if idx == axis else 1 for idx in range(3)]
                input_tensor *= mul_biases.reshape(shape)
                input_tensor = at_norm(input_tensor, self.mul_biases_q.q)
            return input_tensor
        if ktype == "float32":
            if params.has_mul_bias:
                shape = [params.filter.out_c if idx == axis else 1 for idx in range(3)]
                input_tensor *= params.mul_biases.reshape(shape)
            return input_tensor
        raise NotImplementedError()
