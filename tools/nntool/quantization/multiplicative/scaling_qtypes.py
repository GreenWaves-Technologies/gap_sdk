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
from abc import abstractclassmethod, abstractmethod

import numpy as np
from quantization.multiplicative.utils.scale import (apply_scales,
                                                     compute_scales)
from quantization.qtype import QType
from utils.at_norm import at_norm
from utils.event_listener import EventEmitter
from utils.json_serializable import JsonSerializable


DTYPE_CTYPE = {
    np.int8: 'int8',
    np.int16: 'int16',
    np.int32: 'int32',
    np.uint8: 'uint8',
    np.uint16: 'uint16',
    np.uint32: 'uint32',
}


class ScalingQType(JsonSerializable, EventEmitter):
    EXPORT = []

    def __init__(self, *args, dtype=None, **kwargs):
        super(ScalingQType, self).__init__(*args, **kwargs)
        self._scale = None
        self._qnorms = None
        self._qbiases = None
        self._dtype = dtype

    @abstractmethod
    def apply_scales(self, arr: np.ndarray, axis: int = None):
        pass

    @property
    def has_scale(self):
        scale = self.scale
        if isinstance(scale, np.ndarray):
            return np.any(self.scale != 1)
        return self.scale != 1

    @property
    def dtype(self):
        return self._dtype

    @property
    def qnorms(self):
        return self._qnorms

    @property
    def qbiases(self):
        return self._qbiases

    @property
    def scale(self):
        return self._scale

    @property
    def q(self):
        return 0

    @abstractclassmethod
    def from_filter(cls, in_q, weights_q, out_q, params, dtype=None):
        pass

    @property
    def bits(self):
        iinfo = np.iinfo(self.dtype)
        return int(iinfo.bits)

    @property
    def ctype(self):
        return DTYPE_CTYPE[self.dtype]

    def _encapsulate(self):
        return {k: getattr(self, f'_{k}') for k in self.EXPORT
                if getattr(self, f'_{k}') is not None}

    @classmethod
    def _dencapsulate(cls, val):
        return cls(**val)


class MultMulBiasScaleQType(ScalingQType):
    EXPORT = [
        'scale',
        'pre_normalization',
        'dtype',
    ]

    def __init__(self, *args, scale=None, dtype=np.uint8, available_bits=8, **kwargs):
        super(MultMulBiasScaleQType, self).__init__(*args, dtype=dtype, **kwargs)
        self._available_bits = available_bits
        self._pre_normalization = 0
        self.scale = scale

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
        return self._qnorms - self.pre_normalization

    @property
    def pre_normalization(self):
        return self._pre_normalization

    @pre_normalization.setter
    def pre_normalization(self, val):
        self._pre_normalization = val

    @staticmethod
    def reshape_transpose(trans, dim, val):
        return np.reshape(np.transpose(np.reshape(val, dim.shape), trans), val.shape)

    def reorder(self, trans, dim):
        if self.scale.size > 1:
            self.scale = self.reshape_transpose(trans, dim, self.scale)

    @property
    def scale(self):
        return self._scale

    @scale.setter
    def scale(self, val):
        if val is not None:
            if not isinstance(val, np.ndarray):
                val = np.array([val])
            assert np.all(val >= 0), "scale should be positive"
            self._scale = val
            self.compute_scales()
        else:
            self._scale = val

    def compute_scales(self):
        if not self.has_scale:
            self._qnorms = np.array([0], dtype=np.uint8)
            self._qbiases = np.array([1], dtype=np.uint8)
            return
        self._qbiases, self._qnorms = compute_scales(self.scale,
                                                     available_bits=self._available_bits,
                                                     dtype=self.dtype)

    def apply_scales(self, arr: np.ndarray, axis: int = None):
        if self.pre_normalization > 0:
            arr = at_norm(arr, self.pre_normalization)
        if not self.has_scale:
            return arr
        return apply_scales(self.qbiases,
                            self.qnorms,
                            arr,
                            axis=axis)

    def str_by_chan(self, chan: int):
        return "{}b>>{} {:0.3f}".format(self.bits, self.qnorms[chan], self.qbiases[chan])

    def __str__(self):
        qnorms = self.qnorms
        if len(self.qnorms) == 1:
            return "{}b>>{} {:0.3f}".format(self.bits, qnorms[0], self.qbiases[0])
        return "{}b>>chan".format(self.bits)


class MultFractionalMulBiasQType(ScalingQType):
    EXPORT = [
        'scale',
        'dtype',
    ]

    def __init__(self, *args, scale=None, **kwargs):
        super(MultFractionalMulBiasQType, self).__init__(*args, dtype=np.uint32, **kwargs)
        self._scale = None
        self.scale = scale

    @classmethod
    def from_filter(cls, in_q, weights_q, out_q, params, dtype=None):
        return cls(scale=in_q.scale * weights_q.scale / out_q.scale)

    @property
    def scale(self):
        return self._scale

    @scale.setter
    def scale(self, val):
        if val is not None:
            if not isinstance(val, np.ndarray):
                val = np.array([val])
            assert np.all(val >= 0) and np.all(val <= 1), "scale should be positive and fractional"
            self._scale = val
            self.compute_scales()
        else:
            self._scale = val

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
        self._qnorms = qnorms
        #qmults = np.floor(self.scale * math.pow(2, 32) + 0.5)
        iinfo = np.iinfo(self.dtype)
        self._qbiases = np.minimum(qbiases, iinfo.max).astype(self.dtype)

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
