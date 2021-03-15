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

from utils.json_serializable import JsonSerializable

from quantization.qtype import QType


class QuantizationRecordBase(JsonSerializable):
    TYPE = "__not_set__!!"
    DEFAULT = None
    def __init__(self, info=None):
        if info is None:
            self._info = {}
        else:
            self._info = info

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

    def clone_quantization_parameters(self, old_qrec):
        pass

    @staticmethod
    def default_scheme(cls):
        setattr(cls, 'DEFAULT', cls)
        return cls

    def __hash__(self):
        return object.__hash__(self)

default_scheme = QuantizationRecordBase.default_scheme

class InputQuantizationRecordBase(QuantizationRecordBase):

    def __init__(self, *args, in_qs: QType = None, info=None, **kwargs):
        super(InputQuantizationRecordBase, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['in_qs'] = in_qs

    @abstractmethod
    def prepare_inputs(self,
                       params,
                       input_tensors: Sequence[np.ndarray], ktype: str = None) -> Sequence[np.ndarray]:
        """Prepares the inputs before calculation"""

    @property
    def in_qs(self) -> Sequence[QType]:
        if 'in_qs' in self._info:
            return self._info['in_qs']
        return None

    @in_qs.setter
    def in_qs(self, value: Sequence[QType]):
        self._info['in_qs'] = value

    @abstractproperty
    def auto_quantize_inputs(self):
        """Do whatever is necessary to quantize float inputs"""

    @abstractmethod
    @auto_quantize_inputs.setter
    def auto_quantize_inputs(self, val):
        pass

    def clone_quantization_parameters(self, old_qrec):
        super(InputQuantizationRecordBase, self).clone_quantization_parameters(old_qrec)
        self.in_qs = old_qrec.in_qs

    def __str__(self):
        return "i:({})".format(self.ql_str(self.in_qs))


class OutputQuantizationRecordBase(QuantizationRecordBase):

    def __init__(self, *args, out_qs: QType = None, info=None, **kwargs):
        super(OutputQuantizationRecordBase, self).__init__(*args, info=info, **kwargs)
        if info is None:
            self._info['out_qs'] = out_qs

    @abstractmethod
    def get_outputs(self,
                    params,
                    output_tensors: Sequence[np.ndarray],
                    ktype: str = None) -> Sequence[np.ndarray]:
        """Produces the output from the result"""

    @property
    def out_qs(self) -> Sequence[QType]:
        if 'out_qs' in self._info:
            return self._info['out_qs']
        return None

    @out_qs.setter
    def out_qs(self, value: Sequence[QType]):
        self._info['out_qs'] = value

    @abstractproperty
    def auto_dequantize_outputs(self):
        """Do whatever is necessary to dequantize outputs to float"""

    @abstractmethod
    @auto_dequantize_outputs.setter
    def auto_dequantize_outputs(self, val):
        pass

    def clone_quantization_parameters(self, old_qrec):
        super(OutputQuantizationRecordBase, self).clone_quantization_parameters(old_qrec)
        self.out_qs = old_qrec.out_qs

    def __str__(self):
        return "o:({})".format(self.ql_str(self.out_qs))


class ConstantQuantizationRecordBase(OutputQuantizationRecordBase):

    @abstractmethod
    def gen_value(self, value) -> np.ndarray:
        """does whatever is necessary to return real value that can be dumped"""

class InputOutputQuantizationRecordBase(InputQuantizationRecordBase, OutputQuantizationRecordBase):

    def __init__(self, *args, info=None, **kwargs):
        super(InputOutputQuantizationRecordBase, self).__init__(*args, info=info, **kwargs)

    def __str__(self):
        return "i:({}) o:({})".format(self.ql_str(self.in_qs), self.ql_str(self.out_qs))


class FilterQuantizationRecordBase(InputOutputQuantizationRecordBase):
    pass

class ScalableFilterQuantizationRecordBase(FilterQuantizationRecordBase):

    @abstractmethod
    def gen_mul_biases(self, params) -> np.ndarray:
        """Returns the actual multiplicative biases for the filter"""

    @abstractmethod
    def apply_multiplicative_bias(self, params, input_tensor: np.ndarray, axis: int, ktype: str = None):
        """Applies the multiplicative bias during an internel kernel operation on axis of tensor. The mulbias
        may be a function of the quantization or may be in the params."""
