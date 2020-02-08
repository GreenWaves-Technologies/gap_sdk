# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import numpy as np

from utils.json_serializable import JsonSerializable

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
    return obj >> n_bits

class QType(QTypeBase, JsonSerializable):
    def __init__(self, *args, bits=None, q=None, signed=None):
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

    def double_precision(self):
        return QType(self.bits * 2, self.q, self.signed)

    def quantize(self, arr):
        arr = np.floor((arr * 2.0 ** self.q) + 0.5)
        max_value = 2**(self.bits - 1) - 1
        min_value = -max_value - 1
        arr = np.clip(arr, min_value, max_value)
        return np.array(arr, copy=True, dtype=self.dtype)

    def dequantize(self, arr):
        return arr / (2.0**self.q)

    def expand_normalize(self, arr: np.ndarray, cur_qtype: 'QType'):
        assert cur_qtype.length <= self.length, "must expand into something bigger"
        return normalize(arr.astype(self.dtype), cur_qtype.q - self.q)

    def clip(self, arr: np.array, change_type=True):
        min_v, max_v = max_min(self.bits, self.signed)
        ret = np.clip(arr, min_v, max_v)
        if change_type:
            ret = ret.astype(self.dtype)
        return ret

    def round_normalize(self, arr, cur_qtype: 'QType'):
        scale = cur_qtype.q - self.q
        # arr = arr + (1<<(scale - 1))
        arr = normalize(arr, scale)
        return arr

    def round_normalize_clip(self, arr, from_qtype, change_type=True):
        to_qtype = self
        scale = from_qtype.q - to_qtype.q
        # if scale > 0:
        #     arr = arr + (1<<(scale - 1))
        arr = normalize(arr, scale)
        arr = self.clip(arr, change_type)
        return arr

    def expand_from(self, arr, from_qtype):
        return self.expand_normalize(arr, from_qtype)

    def reduce_from(self, arr, from_qtype):
        return self.round_normalize_clip(arr, from_qtype)

    def __eq__(self, other):
        return self.q == other.q and\
            self.bits == other.bits and self.signed == other.signed

    def __str__(self):
        return "Q{}.{}".format(self.bits - self.q, self.q)
