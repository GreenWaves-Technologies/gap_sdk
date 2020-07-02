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

import numpy as np

from quantization.multiplicative.symmetric.symmetric_mult_qtype import SymmetricMultQType
from quantization.multiplicative.asymmetric.asymmetric_mult_qtype import AsymmetricMultQType
from quantization.multiplicative.mult_qtype_base import WrapperMixin

VALID_DTYPES = [
    np.int8,
    np.int16,
    np.int32
]

UNSIGNED_TO_SIGNED = {
    np.uint8: np.int8,
    np.uint16: np.int16,
    np.uint32: np.int32,
    np.int8: np.int8,
    np.int16: np.int16,
    np.int32: np.int32,
}

class SymmetricMultQTypeWrapper(WrapperMixin, SymmetricMultQType):
    def __init__(self, wrapped, *args, scale=None, dtype=None, **kwargs):
        # need to set wrapped before constructor
        self._wrapped = wrapped
        super(SymmetricMultQTypeWrapper, self).__init__(*args, **kwargs)
        self._info['scale'] = scale
        self._info['dtype'] = dtype

    def dtype_is_valid(self):
        return self.dtype in VALID_DTYPES

    def _encapsulate(self):
        #pylint: disable=protected-access
        return {
            "wrapped": AsymmetricMultQType._encapsulate(self._wrapped),
            "scale": self.scale,
            "dtype": self.dtype.__name__ if self.dtype is not None else None
        }

    @classmethod
    def _dencapsulate(cls, val):
        #pylint: disable=protected-access
        dtype = getattr(np, val['dtype']) if val['dtype'] is not None else None
        return cls(AsymmetricMultQType._dencapsulate(val['wrapped']), scale=val['scale'], dtype=dtype)

    @property
    def min_val(self):
        return self._wrapped.min_val

    @min_val.setter
    def min_val(self, val):
        pass

    @property
    def max_val(self):
        return self._wrapped.max_val

    @max_val.setter
    def max_val(self, val):
        pass

    @property
    def scale(self):
        if self._info['scale'] is None:
            # return the bits from the wrapped type. The dtype is only set
            # by the importer to keep the container size between layers
            # fused with activations and should not change the scale of the
            # output.
            if self.range is None:
                return None
            return self.range / (math.pow(2, self._wrapped.bits) - 1)
        return self._info['scale']

    def get_dequantized(self, arr, container_is_quantized=True):
        if container_is_quantized:
            return self._wrapped.dequantize(arr)
        return arr

    def get_quantized(self, arr: np.array, container_is_quantized=True) -> np.array:
        if container_is_quantized:
            return self.quantize(self._wrapped.dequantize(arr))
        return self.quantize(arr)

    @scale.setter
    def scale(self, val):
        # scale can be overriden (this is the case in biases for example)
        self._info['scale'] = val

    @property
    def quantized_dimension(self):
        return self._wrapped.quantized_dimension

    @quantized_dimension.setter
    def quantized_dimension(self, val):
        self._wrapped.quantized_dimension = val

    @property
    def dtype(self):
        if self._info['dtype'] is not None:
            return self._info['dtype']
        return UNSIGNED_TO_SIGNED[self.wrapped.dtype]

    @dtype.setter
    def dtype(self, val):
        self._info['dtype'] = val
