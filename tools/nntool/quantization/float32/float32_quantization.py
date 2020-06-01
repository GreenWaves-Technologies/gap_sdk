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

#pylint: disable=no-self-use
#pylint: disable=too-many-ancestors

from typing import Sequence

import numpy as np

from graph.types import (Conv2DParameters, MultiplicativeBiasParameters,
                         Parameters)
from quantization.quantization_record_base import (QuantizationRecordBase,
                                                   FilterQuantizationRecordBase, InputOutputQuantizationRecordBase,
                                                   ScalableFilterQuantizationRecordBase)


class ShouldNotBeCalledError(Exception):
    pass


class QuantizationMixin(QuantizationRecordBase):
    def prepare_inputs(self,
                       params: Parameters,
                       input_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        del params
        assert ktype == "float32", "incorrect kernel functions for this qrec type"
        return input_tensors

    def get_outputs(self,
                    params: Parameters,
                    output_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        del params
        assert ktype == "float32", "incorrect kernel functions for this qrec type"
        return output_tensors

    @property
    def auto_dequantize_outputs(self):
        return False

    @auto_dequantize_outputs.setter
    def auto_dequantize_outputs(self, val):
        raise ShouldNotBeCalledError()

    @property
    def auto_quantize_inputs(self):
        return False

    @auto_quantize_inputs.setter
    def auto_quantize_inputs(self, val):
        raise ShouldNotBeCalledError()

    @property
    def constants_are_quantized(self) -> bool:
        return False


class Float32QuantizationRecord(QuantizationMixin, InputOutputQuantizationRecordBase):
    pass


class FilterQuantizationMixin(QuantizationMixin):

    def gen_weights(self, params: Parameters, weights: np.ndarray) -> np.ndarray:
        raise ShouldNotBeCalledError()

    def gen_biases(self, biases: np.ndarray) -> np.ndarray:
        raise ShouldNotBeCalledError()

    def prepare_weights(self, params: Parameters, weights: np.ndarray, ktype: str = None) -> np.ndarray:
        del params
        assert ktype == "float32", "incorrect kernel functions for this qrec type"
        return weights.astype(np.float32)

    def prepare_biases(self, params: Parameters, biases: np.ndarray,
                       weights: np.ndarray, ktype: str = None) -> np.ndarray:
        del params, weights
        assert ktype == "float32", "incorrect kernel functions for this qrec type"
        return biases.astype(np.float32)


class Float32FilterQuantizationRecord(FilterQuantizationMixin, FilterQuantizationRecordBase):
    pass


class Float32ScalableFilterQuantizationRecord(FilterQuantizationMixin, ScalableFilterQuantizationRecordBase):

    def gen_mul_biases(self, params: MultiplicativeBiasParameters) -> np.ndarray:
        raise ShouldNotBeCalledError()

    def apply_multiplicative_bias(self,
                                  params: Conv2DParameters,
                                  input_tensor: np.ndarray,
                                  axis: int,
                                  ktype: str = None):
        assert ktype == "float32", "incorrect kernel functions for this qrec type"
        if params.has_mul_bias:
            shape = [params.filter.out_c if idx == axis else 1 for idx in range(3)]
            input_tensor *= params.mul_biases.reshape(shape)
        return input_tensor
