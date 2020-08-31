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
from abc import abstractmethod, abstractclassmethod

import numpy as np

from graph.types import FilterParameters
from quantization.multiplicative.symmetric.symmetric_mult_qtype import \
    SymmetricMultQType
from quantization.qtype import QType
from utils.at_norm import at_norm

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


class MultMulBiasQType(SymmetricMultQType):
    @abstractmethod
    def apply_scales(self, arr: np.ndarray, axis: int = None):
        pass

    @property
    def has_scale(self):
        scale = self.scale
        if isinstance(scale, np.ndarray):
            return np.any(self.scale != 1)
        return self.scale != 1

    @abstractclassmethod
    def from_filter(cls, in_q, weights_q, out_q, params, dtype=None):
        pass

class MultMulBiasScaleQType(MultMulBiasQType):
    def __init__(self, *args, dtype=np.uint8, available_bits=8, init=None, **kwargs):
        self._available_bits = available_bits
        super(MultMulBiasScaleQType, self).__init__(*args, dtype=dtype, init=init, **kwargs)
        if init:
            self._info = init

    @classmethod
    def from_filter(cls, in_q, weights_q, out_q, params, dtype=np.uint8):
        available_bits = (
            31 - (math.ceil(math.log2(params.filter.sz)) + 7 + 7))
        qtype = cls(dtype=dtype, available_bits=available_bits)
        qtype.scale = in_q.scale * weights_q.scale / out_q.scale
        return qtype

    @property
    def shift_ctype(self):
        return "int8"

    @property
    def shift_qtype(self):
        return QType(q=0, bits=8, signed=True)

    @property
    def qnorms(self):
        return self._info['qnorms'] - self.pre_normalization

    @property
    def qbiases(self):
        return self._info['qbiases']

    @property
    def pre_normalization(self):
        return self._info.get('pre_normalization') or 0

    @pre_normalization.setter
    def pre_normalization(self, val):
        self._info['pre_normalization'] = val

    def _encapsulate(self):
        return {
            "qnorms": self.qnorms,
            "qbiases": self.qbiases,
            "scale": self.scale,
            "pre_normalization": self.pre_normalization,
            "dtype": self.dtype.__name__
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init={
            "qnorms": val['qnorms'],
            "qbiases": val['qbiases'],
            "scale": val['scale'],
            "pre_normalization": val['pre_normalization'] if 'pre_normalization' in val else None,
            "dtype": getattr(np, val['dtype'])
        })

    @property
    def scale(self):
        return self._info['scale']

    @scale.setter
    def scale(self, val):
        if val is not None:
            if not isinstance(val, np.ndarray):
                val = np.array([val])
            assert np.all(val >= 0), "scale should be positive"
            self._info['scale'] = val
            self.compute_scales()
        else:
            self._info['scale'] = val

    def compute_scales(self):
        if not self.has_scale:
            self._info['qnorms'] = np.array([1], dtype=np.uint8)
            self._info['qbiases'] = np.array([0], dtype=np.uint8)
            return
        if self.dtype == np.int8:
            max_bits = min(7, self._available_bits)
        elif self.dtype == np.uint8:
            max_bits = min(8, self._available_bits)
        bits = np.full(self.scale.shape, max_bits)
        factors = np.array([math.frexp(scale) for scale in self.scale],
                           dtype=[("scale", "f4"), ("norm", "i1")])

        max_val = np.power(2, max_bits)
        while True:
            qbiases = np.floor(factors['scale'] * np.power(2, bits) + 0.5)
            qnorms = -(factors["norm"] - bits)
            # overflow in conversion
            max_exceeded = qbiases >= max_val
            # shifting away bits
            norms_too_high = qnorms > 32 - 8
            # mult * pow 2 then shift
            bias_pow2 = qbiases % 2 == 0
            should_move = np.logical_or(max_exceeded, np.logical_or(norms_too_high, bias_pow2))
            can_still_move = np.logical_and(qnorms > 0, bits > 0)
            overflow = np.logical_and(should_move, can_still_move)
            if not np.any(overflow):
                break
            bits[overflow] -= 1

        self._info['qnorms'] = qnorms.astype(np.uint8)
        self._info['qbiases'] = qbiases.astype(self.dtype)

    def apply_scales(self, arr: np.ndarray, axis: int = None):
        if self.pre_normalization > 0:
            arr = at_norm(arr, self.pre_normalization)
        if not self.has_scale:
            return arr
        if axis is None:
            mul_biases = self.qbiases
            mul_biases_norm = self.qnorms
            assert len(mul_biases) == 1 and len(
                mul_biases_norm) == 1, "no axis set. should have single scale"
        else:
            shape = [len(self.qbiases) if idx == axis else 1 for idx in range(len(arr.shape))]
            mul_biases = self.qbiases.reshape(shape)
            mul_biases_norm = self.qnorms.reshape(shape)
        return at_norm(np.multiply(arr, mul_biases, dtype=np.int32), mul_biases_norm)

    def str_by_chan(self, chan: int):
        return "{}b>>{} {:0.3f}".format(self.bits, self.qnorms[chan], self.qbiases[chan])

    def __str__(self):
        qnorms = self.qnorms
        if len(self.qnorms) == 1:
            return "{}b>>{} {:0.3f}".format(self.bits, qnorms[0], self.qbiases[0])
        return "{}b>>chan".format(self.bits)


class MultFractionalMulBiasQType(MultMulBiasQType):
    def __init__(self, *args, init=None, **kwargs):
        kwargs['dtype'] = np.uint32
        # force to uint32
        super(MultFractionalMulBiasQType, self).__init__(*args, init=init, **kwargs)
        if init:
            self._info = init

    @classmethod
    def from_filter(cls, in_q, weights_q, out_q, params, dtype=None):
        return cls(scale=in_q.scale * weights_q.scale / out_q.scale)

    @property
    def qnorms(self):
        return self._info['qnorms']

    @property
    def qbiases(self):
        return self._info['qbiases']

    def _encapsulate(self):
        return {
            "qbiases": self.max_val.tolist() if self.qbiases else None,
            "scale": self.scale.tolist() if self.scale else None,
            "dtype": self.dtype.__name__
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(None, None, init={
            "qbiases": cls.init_array(val, 'qbiases'),
            "scale": cls.init_array(val, 'scale'),
            "dtype": getattr(np, val['dtype'])
        })

    @property
    def scale(self):
        return self._info['scale']

    @scale.setter
    def scale(self, val):
        if val is not None:
            if not isinstance(val, np.ndarray):
                val = np.array([val])
            assert np.all(val >= 0) and np.all(val <= 1), "scale should be positive and fractional"
            self._info['scale'] = val
            self.compute_scales()
        else:
            self._info['scale'] = val

    def compute_scales(self):
        if not self.has_scale:
            return
        factors = np.array([math.frexp(scale) for scale in self.scale],
                           dtype=[("scale", "f4"), ("norm", "i1")])
        qbiases = np.floor(factors['scale'] * math.pow(2, 32) + 0.5)
        qnorms = -(factors["norm"])
        overflow = qbiases >= math.pow(2, 32)
        qnorms[overflow] -= 1
        qbiases[overflow] //= 2
        self._info['qnorms'] = qnorms
        #qmults = np.floor(self.scale * math.pow(2, 32) + 0.5)
        iinfo = np.iinfo(self.dtype)
        self._info['qbiases'] = np.minimum(qbiases, iinfo.max).astype(self.dtype)

    def apply_scales(self, arr: np.ndarray, axis: int = None):
        if not self.has_scale:
            return arr.astype(np.int32)
        if axis is None:
            mul_biases = self.qbiases
            mul_biases_norm = self.qnorms
            assert len(mul_biases) == 1 and len(
                mul_biases_norm) == 1, "no axis set. should have single scale"
        else:
            shape = [len(self.qbiases) if idx == axis else 1 for idx in range(len(arr.shape))]
            mul_biases = self.qbiases.reshape(shape)
            mul_biases_norm = self.qnorms.reshape(shape)

        #arr = np.multiply(arr, mul_biases, dtype=np.int64) >> 32
        arr = at_norm(np.multiply(arr, mul_biases, dtype=np.int64), 32 + mul_biases_norm)
        return arr.astype(np.int32)

    def str_by_chan(self, chan: int):
        return "{}b{:0.6f}".format(self.bits, self.qbiases[chan])

    def __str__(self):
        qbiases = self.qbiases
        if len(self.qbiases) == 1:
            return "{}b{:0.6f}".format(self.bits, qbiases[0])
        return "{}bchan".format(self.bits)
