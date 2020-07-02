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

#pylint: disable=abstract-method

from abc import abstractmethod, abstractproperty
from typing import Sequence

import numpy as np

from graph.types import Parameters, MultiplicativeBiasParameters
from utils.json_serializable import JsonSerializable

from quantization.qtype_base import QTypeBase
from quantization.multiplicative.mult_qtype_base import WrapperMixin


class QuantizationRecordBase(JsonSerializable):
    def __init__(self, info=None):
        self._unwrap = False
        if info is None:
            self._info = {}
        else:
            self._info = info

    @property
    def unwrap(self):
        return self._unwrap

    @unwrap.setter
    def unwrap(self, val):
        self._unwrap = val

    def unwrapped(self, val):
        if self.unwrap:
            if isinstance(val, list):
                return [self.unwrapped(v) for v in val]
            if isinstance(val, WrapperMixin):
                return val.wrapped
        return val

    def _encapsulate(self):
        return self._info

    @classmethod
    def _dencapsulate(cls, val):
        return cls(info=val)

    def __eq__(self, value):
        # pylint: disable=protected-access
        return value._info == self._info

    @staticmethod
    def ql_str(l):
        return ",".join([str(qtype) for qtype in l])

    def __hash__(self):
        return object.__hash__(self)

class InputQuantizationRecordBase(QuantizationRecordBase):

    def __init__(self, *args, in_qs: QTypeBase = None, info=None, **kwargs):
        super(InputQuantizationRecordBase, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['in_qs'] = in_qs

    @abstractmethod
    def prepare_inputs(self,
                       params: Parameters,
                       input_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        """Prepares the inputs before calculation"""

    @property
    def in_qs(self) -> Sequence[QTypeBase]:
        if 'in_qs' in self._info:
            return self.unwrapped(self._info['in_qs'])
        return None

    @in_qs.setter
    def in_qs(self, value: Sequence[QTypeBase]):
        self._info['in_qs'] = value

    @abstractproperty
    def auto_quantize_inputs(self):
        """Do whatever is necessary to quantize float inputs"""

    @abstractmethod
    @auto_quantize_inputs.setter
    def auto_quantize_inputs(self, val):
        pass

    def __str__(self):
        return "i:({})".format(self.ql_str(self.in_qs))


class OutputQuantizationRecordBase(QuantizationRecordBase):

    def __init__(self, *args, out_qs: QTypeBase = None, info=None, **kwargs):
        super(OutputQuantizationRecordBase, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['out_qs'] = out_qs

    @abstractmethod
    def get_outputs(self,
                    params: Parameters,
                    output_tensors: Sequence[np.ndarray],
                    ktype: str = None) -> Sequence[np.ndarray]:
        """Produces the output from the result"""

    @property
    def out_qs(self) -> Sequence[QTypeBase]:
        if 'out_qs' in self._info:
            return self.unwrapped(self._info['out_qs'])
        return None

    @out_qs.setter
    def out_qs(self, value: Sequence[QTypeBase]):
        self._info['out_qs'] = value

    @abstractproperty
    def auto_dequantize_outputs(self):
        """Do whatever is necessary to dequantize outputs to float"""

    @abstractmethod
    @auto_dequantize_outputs.setter
    def auto_dequantize_outputs(self, val):
        pass

    def __str__(self):
        return "o:({})".format(self.ql_str(self.out_qs))


class HasConstantsBase(OutputQuantizationRecordBase):
    def __init__(self, *args, constants_are_quantized: bool = True, info=None, **kwargs):
        super(HasConstantsBase, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['constants_are_quantized'] = constants_are_quantized

    @property
    def constants_are_quantized(self) -> bool:
        return self._info['constants_are_quantized']

    @constants_are_quantized.setter
    def constants_are_quantized(self, val: bool):
        self._info['constants_are_quantized'] = val


class ConstantQuantizationRecordBase(HasConstantsBase):

    @abstractmethod
    def gen_value(self, value) -> np.ndarray:
        """does whatever is necessary to return real value that can be dumped"""

class InputOutputQuantizationRecordBase(InputQuantizationRecordBase, OutputQuantizationRecordBase):

    def __init__(self, *args, info=None, **kwargs):
        super(InputOutputQuantizationRecordBase, self).__init__(*args, info=info, **kwargs)

    def __str__(self):
        return "i:({}) o:({})".format(self.ql_str(self.in_qs), self.ql_str(self.out_qs))


class FilterQuantizationRecordBase(InputOutputQuantizationRecordBase, HasConstantsBase):
    """The base FilterQuantization record does not have weights_q and biases_q since
    they may not be needed. It does however have properties for code generation.
    __str__ needs to be overidden if there is more info to print about these. So it
    has no constructor by default."""

    @abstractmethod
    def gen_weights(self, params: Parameters, weights) -> np.ndarray:
        """does whatever is necessary to return real weights that can be dumped"""

    @abstractmethod
    def gen_biases(self, params: Parameters, biases: np.ndarray, weights: np.ndarray) -> np.ndarray:
        """does whatever is necessary to return real biases that can be dumped"""

    @abstractmethod
    def prepare_weights(self, params, weights: np.ndarray, ktype: str = None) -> np.ndarray:
        """Prepares the weights before calculation"""

    @abstractmethod
    def prepare_biases(self, params, biases: np.ndarray, weights: np.ndarray, ktype: str = None) -> np.ndarray:
        """Prepares the biases before calculation"""

class ScalableFilterQuantizationRecordBase(FilterQuantizationRecordBase):

    @abstractmethod
    def gen_mul_biases(self, params: MultiplicativeBiasParameters) -> np.ndarray:
        """Returns the actual multiplicative biases for the filter"""

    @abstractmethod
    def apply_multiplicative_bias(self, params: Parameters, input_tensor: np.ndarray, axis: int, ktype: str = None):
        """Applies the multiplicative bias during an internel kernel operation on axis of tensor. The mulbias
        may be a function of the quantization or may be in the params."""
