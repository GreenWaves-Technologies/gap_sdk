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

from quantization.qtype_base import QTypeBase
from utils.json_serializable import JsonSerializable

NUM_BITS = {
    np.int8: 8,
    np.int16: 16,
    np.int32: 32,
    np.uint8: 8,
    np.uint16: 16,
    np.uint32: 32,
}

SIGNED = [
    np.int8,
    np.int16,
    np.int32
]

class MultMulBiasQType(QTypeBase, JsonSerializable):
    def __init__(self, dtype=None, init=None):
        if init:
            self._info = init
        else:
            self._info = {
                'dtype': dtype if dtype is not None else np.int16,
                'shift': None
            }

    @property
    def q(self):
        return self._info['shift']

    @property
    def bits(self):
        return NUM_BITS[self.dtype]

    @property
    def signed(self):
        return self.dtype in SIGNED

    @property
    def shift_is_set(self):
        return self.q is not None

    @property
    def dtype(self):
        return self._info['dtype']

    @property
    def pad_zero_point(self):
        return 0

    def _encapsulate(self):
        return {
            'dtype': self.dtype.__name__,
            'shift': self.q
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init={
            'dtype': getattr(np, val['dtype']),
            'shift': val['shift']
        })

    @property
    def pre_normalization(self):
        return self._info.get('pre_normalization') or 0

    @pre_normalization.setter
    def pre_normalization(self, val):
        self._info['pre_normalization'] = val

    def quantize_elem(self, val: np.float64):
        return self.normalize(val)

    def quantize(self, arr: np.array) -> np.array:
        return np.array([self.normalize(elem) for elem in arr]).astype(self.dtype)

    def dequantize_elem(self, val):
        return val * 1.0/(1 << self.q)

    def dequantize(self, arr: np.array) -> np.array:
        return arr.astype(np.float32) * 1/(1 << self.q)

    def get_shift(self, max_num):
        (val, shift) = math.frexp(max_num)
        if val > ((self.bits - 2)/(self.bits - 1)):
            val /= 2
            shift += 1
        shift = shift - (self.bits - 1)
        return val, shift

    def set_shift(self, max_num):
        _, shift = self.get_shift(max_num)
        shift = shift - self.pre_normalization
        assert shift <= 0, "number cannot be represented with a right shift"
        self._info['shift'] = -shift

    def normalize(self, fnum):
        val, shift = self.get_shift(fnum)
        cor = shift + self.q - self.pre_normalization
        assert cor <= 0, "correction should never be positive"
        return math.floor((val * (1 << (self.bits - 1 + cor))) + 0.5)

    def __str__(self):
        return "{}b>>{}".format(self.bits, self.q)
