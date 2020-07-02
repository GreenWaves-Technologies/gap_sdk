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

from utils.json_serializable import JsonSerializable
from utils.at_norm import at_norm
# from utils.stats_funcs import bits as min_max_to_bits


from .qtype_base import QTypeBase

C_TYPES = {
    True: {
        8: "signed char",
        16: "short int",
        32: "int"
    },
    False: {
        8: "unsigned char",
        16: "unsigned short int",
        32: "unsigned int"
    }
}

DTYPES = {
    np.uint8: (8, False),
    np.uint16: (16, False),
    np.uint32: (32, False),
    np.int8: (8, True),
    np.int16: (16, True),
    np.int32: (32, True),
}


def get_dtype(length, signed):
    if signed:
        return np.dtype("i"+str(int(length)))
    return np.dtype("u"+str(length))

def max_min(n_bits, signed):
    if signed:
        max_v = (1 << (n_bits - 1)) - 1
        min_v = -max_v - 1
    else:
        min_v = 0
        max_v = (1 << n_bits) - 1
    return min_v, max_v

def normalize(obj, n_bits):
    if n_bits == 0:
        return obj
    if n_bits < 0:
        return obj << -n_bits
    return at_norm(obj, n_bits)

def calc_int_bits(arr, signed=True):
    abs_num = np.floor(np.abs(arr))
    # calculate number of bits to represent absolute number
    if signed:
        if abs_num == 0:
            return 1
        return np.ceil(np.log(abs_num) / np.log(2)) + 1
    else:
        if abs_num == 0:
            return 0
        return np.ceil(np.log(abs_num) / np.log(2))

class QType(QTypeBase, JsonSerializable):
    def __init__(self, *args, bits=None, q=None, signed=None, dtype=None):
        if args:
            if isinstance(args[0], QType):
                proto = args[0]
                self._quant = [proto.bits, proto.q, proto.signed]
            elif len(args) == 3:
                self._quant = args
            else:
                raise ValueError()
        else:
            self._quant = [None, None, None]

        if bits is not None:
            self._quant[0] = bits

        if q is not None:
            self._quant[1] = q

        if signed is not None:
            self._quant[2] = signed

        if dtype is not None:
            self._quant[0], self._quant[2] = DTYPES[dtype]

    # @classmethod
    # def from_array(cls, arr: np.ndarray, bits, signed=True):
    #     int_bits = min_max_to_bits(arr.max(), arr.min())
    #     assert int_bits + (1 if signed else 0) <= bits, "number cannot be represented with this many bits"
    #     return cls(bits=bits, q=bits - int_bits, signed=signed)

    def _encapsulate(self):
        return self._quant

    @classmethod
    def _dencapsulate(cls, val):
        return QType(*val)

    def increase_precision(self):
        return QType(self.bits * 2, self.q, self.signed)

    def clone(self):
        return QType(self.bits, self.q, self.signed)

    @property
    def can_increase_precision(self):
        return self.bits != 32

    @staticmethod
    def none():
        return QType(None, None, None)

    @property
    def is_quantized(self):
        return not all(i is None for i in self._quant)

    @property
    def ctype(self):
        return C_TYPES[self.signed][self.bits]

    @property
    def dtype(self):
        return get_dtype(self.bits//8, self.signed)

    @property
    def length(self):
        return self._quant[0] // 8

    @property
    def bits(self):
        return self._quant[0]

    @bits.setter
    def bits(self, val):
        self._quant[0] = val

    @property
    def q(self):
        return self._quant[1]

    @q.setter
    def q(self, val):
        self._quant[1] = val

    @property
    def signed(self):
        return self._quant[2]

    @signed.setter
    def signed(self, val):
        self._quant[2] = val

    @property
    def pad_zero_point(self):
        return 0

    def double_precision(self):
        return QType(self.bits * 2, self.q, self.signed)

    def quantize(self, arr):
        arr = np.floor((arr * 2.0 ** self.q) + 0.5)
        return self.clip(arr)

    def dequantize(self, arr):
        return arr / (2.0**self.q)

    def expand_normalize(self, arr: np.ndarray, cur_qtype: 'QType'):
        assert cur_qtype.length <= self.length, "must expand into something bigger"
        return normalize(arr.astype(self.dtype), cur_qtype.q - self.q)

    def round_normalize(self, arr, cur_qtype: 'QType'):
        scale = cur_qtype.q - self.q
        # arr = arr + (1<<(scale - 1))
        arr = normalize(arr, scale)
        return arr

    def round_normalize_clip(self, arr, from_qtype):
        to_qtype = self
        scale = from_qtype.q - to_qtype.q
        # if scale > 0:
        #     arr = arr + (1<<(scale - 1))
        arr = normalize(arr, scale)
        arr = self.clip(arr)
        return arr

    def expand_from(self, arr, from_qtype):
        return self.expand_normalize(arr, from_qtype)

    def reduce_from(self, arr, from_qtype):
        return self.round_normalize_clip(arr, from_qtype)

    def __add__(self, other):
        return QType(max(self.bits, other.bits), self.q + other.q, self.signed or other.signed)

    def __eq__(self, other):
        if isinstance(other, QType):
            return self.q == other.q and\
                self.bits == other.bits and self.signed == other.signed
        return other.__eq__(self)

    def __str__(self):
        if self.q > self.bits:
            return "M{}>>{}".format(self.bits, self.q)
        return "Q{}.{}".format(self.bits - self.q, self.q)
