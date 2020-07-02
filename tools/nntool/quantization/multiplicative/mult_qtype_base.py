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
import numbers
from abc import ABC, abstractmethod, abstractproperty

import numpy as np

from quantization.qtype import QType
from quantization.qtype_base import QTypeBase
from utils.json_serializable import JsonSerializable

#pylint: disable=abstract-method

SIGNED = [
    np.int8,
    np.int16,
    np.int32
]

DTYPE_STR = {
    np.int8: 'i8',
    np.int16: 'i16',
    np.int32: 'i32',
    np.uint8: 'u8',
    np.uint16: 'u16',
    np.uint32: 'u32',
}

DTYPE_CTYPE = {
    np.int8: 'int8',
    np.int16: 'int16',
    np.int32: 'int32',
    np.uint8: 'uint8',
    np.uint16: 'uint16',
    np.uint32: 'uint32',
}


class WrapperMixin(ABC):
    @property
    def wrapped(self):
        return self._wrapped

    def quantize_wrapped(self, arr: np.array) -> np.array:
        return self._wrapped.quantize(arr)

    def dequantize_wrapped(self, arr: np.array) -> np.array:
        return self._wrapped.dequantize(arr)

    def requantize(self, arr: np.array) -> np.array:
        return self.quantize(self._wrapped.dequantize(arr))


class MultQTypeBase(QTypeBase, JsonSerializable):
    def __init__(self, *args, min_val=None, max_val=None, scale=None,
                 quantized_dimension=0, dtype=None, init=None, **kwargs):
        super(MultQTypeBase, self).__init__(*args, **kwargs)
        if init:
            self._info = init
        else:
            self._info = {}
            self.min_val = self.init_array(min_val)
            self.max_val = self.init_array(max_val)
            self.quantized_dimension = quantized_dimension
            self.dtype = dtype if dtype is not None else np.int8
            self.scale = self.init_array(scale)
        # if not self.dtype_is_valid():
        #     raise ValueError("dtype %s is not valid for %s" % (self.dtype, self.__class__.__name__))


    @property
    def ctype(self):
        return DTYPE_CTYPE[self.dtype]

    @abstractmethod
    def dtype_is_valid(self):
        pass

    @staticmethod
    def init_array(val, key=None):
        if key is not None:
            val = val.get(key)
        if isinstance(val, np.ndarray):
            return val
        elif isinstance(val, (numbers.Real, np.float)):
            return np.array([val])
        elif isinstance(val, list):
            return np.array(val)
        elif val is None:
            return None
        else:
            raise ValueError("invalid scale")

    @property
    def signed(self):
        return self.dtype in SIGNED

    @property
    def q(self):  # in mulbias scheme all Qs are zero except the mulbias
        return 0

    @property
    def bits(self):
        return self.dtype().itemsize * 8

    @abstractproperty
    def zero_point(self):
        pass

    @abstractmethod
    @zero_point.setter
    def zero_point(self, val):
        pass

    @abstractproperty
    def min(self):
        pass

    @property
    def min_val(self):
        return self._info['min_val']

    @min_val.setter
    def min_val(self, val):
        self._info['min_val'] = val

    @abstractproperty
    def max(self):
        pass

    @property
    def max_val(self):
        return self._info['max_val']

    @max_val.setter
    def max_val(self, val):
        self._info['max_val'] = val

    @property
    def range(self):
        if self.max is None and self.min is None:
            return None
        assert np.all(self.max >= self.min)
        return self.max - self.min

    kNearZeroTolerance = 1.0e-6

    @property
    def scale(self):
        return self._info['scale']

    @scale.setter
    def scale(self, val):
        if isinstance(val, np.ndarray):
            self._info['scale'] = np.where(val < self.kNearZeroTolerance, 1, val)
        elif val is not None and val < self.kNearZeroTolerance:
            self._info['scale'] = 1
        else:
            self._info['scale'] = val

    @property
    def quantized_dimension(self):
        return self._info['quantized_dimension']

    @quantized_dimension.setter
    def quantized_dimension(self, val):
        self._info['quantized_dimension'] = val

    @property
    def dtype(self):
        return self._info['dtype']

    @dtype.setter
    def dtype(self, val):
        self._info['dtype'] = val

    @property
    def pad_zero_point(self):
        return 0

    @abstractmethod
    def get_quantized(self, arr, container_is_quantized=True):
        pass

    @abstractmethod
    def get_dequantized(self, arr, container_is_quantized=True):
        pass

    def expand_from(self, arr: np.array, from_qtype: QTypeBase) -> np.array:
        #if self.dtype != from_qtype.dtype:
        return arr.astype(self.dtype)

    def reduce_from(self, arr: np.array, from_qtype: QTypeBase) -> np.array:
        #if self.dtype != from_qtype.dtype:
        return self.clip(arr)

    @staticmethod
    def str_fmt(val, extend=False):
        if val is None:
            return "unkn"
        if isinstance(val, (int, float)) or isinstance(val, (np.floating, np.integer)):
            return val
        return "chan" if len(val) > 1 else ("{:0.2f}".format(val[0]) if not extend else "{:0.8f}".format(val[0]))

    def __eq__(self, other):
        if isinstance(other, QType):
            return self.bits == other.bits
        if isinstance(other, MultQTypeBase):
            return self.scale == other.scale and\
                self.dtype == other.dtype and\
                self.quantized_dimension == other.quantized_dimension and\
                self.zero_point == other.zero_point
        return False

    @property
    def dtype_str(self):
        return DTYPE_STR[self.dtype]

    def str_by_chan(self, chan: int):
        return "{}<({}-{})*{}<{}".format(
            self.str_fmt(self.min[chan]),
            self.dtype_str,
            self.str_fmt(self.zero_point[chan]),
            self.str_fmt(self.scale[chan]),
            self.str_fmt(self.max[chan]),
        )

    def __str__(self):
        return "{}<({}-{})*{}<{}".format(
            self.str_fmt(self.min),
            self.dtype_str,
            self.str_fmt(self.zero_point),
            self.str_fmt(self.scale, extend=True),
            self.str_fmt(self.max),
        )
