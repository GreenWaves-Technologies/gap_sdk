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
from bfloat16 import bfloat16
from utils.at_norm import at_norm
from utils.event_listener import EventEmitter
from utils.json_serializable import JsonSerializable
from utils.stats_funcs import calc_bits

from quantization.multiplicative.mult_utils import compute_mul_bias

DTYPE_STR = {
    np.int8: 'i8',
    np.int16: 'i16',
    np.int32: 'i32',
    np.uint8: 'u8',
    np.uint16: 'u16',
    np.uint32: 'u32',
    bfloat16: 'bf16',
    np.float16: 'f16',
    np.float32: 'f32',
}

STR_DTYPE = {v: k for k, v in DTYPE_STR.items()}

DTYPE_CTYPE = {
    np.int8: 'int8',
    np.int16: 'int16',
    np.int32: 'int32',
    np.uint8: 'uint8',
    np.uint16: 'uint16',
    np.uint32: 'uint32',
    np.float32: 'float32',
    np.float16: 'float16',
    bfloat16: 'bfloat16'
}

DTYPES = {
    np.uint8: (8, False),
    np.uint16: (16, False),
    np.uint32: (32, False),
    np.int8: (8, True),
    np.int16: (16, True),
    np.int32: (32, True),
    np.float16: (16, True),
    bfloat16: (16, True),
    np.float32: (32, True),
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


def divide_ignore(a, b):
    with np.errstate(divide='ignore', invalid='ignore'):
        res = np.true_divide(a, b)
        res[res == np.inf] = 0
        return np.nan_to_num(res)


class AttrNamespace:
    def __init__(self, **kwargs):
        self.__dict__.update(kwargs)

    def __getattr__(self, name: str):
        if name.startswith('__'):
            raise AttributeError()
        return False

    def __setattr__(self, name: str, value) -> None:
        self.__dict__[name] = value

    def __delattr__(self, name: str) -> None:
        if name in self.__dict__:
            del self.__dict__[name]

    def __iter__(self):
        yield from [a for a in dir(self) if not a.startswith('__')]

    def __repr__(self):
        items = (f"{k}={v!r}" for k, v in self.__dict__.items())
        return "({})".format(", ".join(items))

    def __eq__(self, other):
        if isinstance(self, AttrNamespace) and isinstance(other, AttrNamespace):
            return self.__dict__ == other.__dict__
        return NotImplemented

    def __getstate__(self):
        return {k: getattr(self, k) for k in self}

    def __setstate__(self, state):
        self.__dict__.update(state)


FORCED_FLAGS = [
    'q',
    'dtype',
    'zero_point',
    'scale',
]


class QType(JsonSerializable, EventEmitter):
    EXPORT = [
        'q',
        'bits',
        'signed',
        'zero_point',
        'scale',
        'min_val',
        'max_val',
        'quantized_dimension',
        'narrow_range',
        'forced',
        'offset',
        'attr',
        'is_constant'
    ]

    def __init__(self, *args, q=None, bits=None, signed=True, zero_point=0, scale=None,
                 min_val=None, max_val=None, quantized_dimension=None, dtype=None,
                 narrow_range=None, forced=False, dont_copy_attr=None, **kwargs):
        super(QType, self).__init__(*args)
        self._q = q
        self._scale = self.init_array(scale)
        self._min_val = self.init_array(min_val)
        self._max_val = self.init_array(max_val)
        self._zero_point = np.atleast_1d(zero_point)
        self._quantized_dimension = quantized_dimension
        self._narrow_range = narrow_range
        self._forced = {k: True for k in FORCED_FLAGS} if forced else {}
        self._offset = None
        self._attr = AttrNamespace(**kwargs)
        self._is_constant = None
        self._dont_copy_attr = dont_copy_attr
        if bits is None:
            self.dtype = dtype
        elif dtype is None:
            self._bits = bits
            self._signed = signed
            self._update_dtype()
        else:
            self.dtype = dtype
            self.bits = bits

    def make_symmetric_signed(self):
        if self.is_sq and (self.is_asymmetric or not self.signed):
            return QType.from_min_max_sq(self.min_val, self.max_val,
                                         quantized_dimension=self.quantized_dimension)
        return self

    def __getstate__(self):
        state = {k: getattr(self, f'_{k}') for k in self.EXPORT}
        if self._dont_copy_attr:
            for k in self._dont_copy_attr:
                if k in state['attr']:
                    delattr(state['attr'], k)
        state['dtype'] = DTYPE_STR[self.dtype]
        return state

    def __setstate__(self, state):
        for k in self.EXPORT:
            setattr(self, f'_{k}', state[k])
        setattr(self, '_dont_copy_attr', None)
        setattr(self, '_dtype', STR_DTYPE[state['dtype']])
        setattr(self, '_EventEmitter__raw_listeners', {})

    @property
    def zero_point_is_asymmetric_zero(self):
        if self.dtype in [np.int8, np.int16, np.int32]:
            min_val = np.iinfo(self.dtype).min
            return np.all(self.zero_point == min_val)
        if self.dtype in [np.uint8, np.uint16, np.uint32]:
            return np.all(self.zero_point == 0)
        return False

    @property
    def dont_copy_attr(self):
        return self._dont_copy_attr

    @dont_copy_attr.setter
    def dont_copy_attr(self, val):
        self._dont_copy_attr = val

    @property
    def is_fixed_point(self):
        return not self.is_floating

    @property
    def is_constant(self):
        return self._is_constant

    @is_constant.setter
    def is_constant(self, val):
        self._is_constant = val

    @property
    def attr(self):
        return self._attr

    @classmethod
    def Pow2(cls, bits, q, signed, forced=False):
        return cls(bits=bits, q=q, signed=signed, forced=forced)

    def link_scales(self, *others):
        def set_scale(*args):
            del args
            if all(other.is_pow2 for other in others):
                self.q = sum(other.q for other in others)
            else:
                self.scale = np.atleast_1d(
                    np.prod([other.scale for other in others]))
        for other in others:
            other.on('scale_changed', set_scale)
        set_scale()
        return self

    def _encapsulate(self):
        return {k: getattr(self, f'_{k}') for k in self.EXPORT
                if getattr(self, f'_{k}') is not None}

    @classmethod
    def _dencapsulate(cls, val):
        return QType(**val)

    def _update_dtype(self):
        if self._signed is None or self._bits is None:
            raise ValueError('signed and bits must be set')
        bits = self.length * 8
        candidate = None
        for i, (j, k) in DTYPES.items():
            #pylint: disable=unsubscriptable-object
            if k == self._signed and j >= bits and (candidate is None or candidate[1] > j):
                candidate = (i, j)
        if candidate is None:
            raise ValueError('no suitable dtype found')
        self._dtype = candidate[0]

    @property
    def forced(self):
        return any(self._forced.values())

    @property
    def forced_flags(self):
        return self._forced

    @property
    def forced_q(self):
        return self._forced.get('q')

    @property
    def forced_zero_point(self):
        return self._forced.get('zero_point')

    @property
    def forced_dtype(self):
        return self._forced.get('dtype')

    @property
    def forced_scale(self):
        return self._forced.get('scale')

    def set_forced(self, val=True, flags=None):
        if flags is None:
            flags = FORCED_FLAGS
        for flag in flags:
            self._forced[flag] = val
        return self

    def can_force(self, *others):
        for other in others:
            if not all(not other.forced_flags.get(flag) or getattr(self, flag) == getattr(other, flag)
                       for flag in self.forced_flags):
                return False
        return True

    @classmethod
    def forced_equal(cls, *args):
        if len(args) < 2:
            return True
        forced = set([k for qtype in args for k,
                      v in qtype.forced_flags.items() if v])
        return all(all(getattr(args[0], force_flag) == getattr(other, force_flag)
                       for force_flag in forced)
                   for other in args[1::])

    @property
    def is_sq(self):
        return self._scale is not None

    @property
    def is_pow2(self):
        return self._q is not None

    @property
    def is_asymmetric(self):
        if self.is_floating:
            return False
        if self.signed:
            return np.any(self._zero_point != 0)
        return np.any(self._zero_point != np.ceil((np.power(2, self.bits) - 1)/2).astype(self.dtype))

    @property
    def is_symmetric(self):
        return not self.is_asymmetric

    @property
    def offset(self):
        return self._offset

    @offset.setter
    def offset(self, val):
        self._offset = val

    @property
    def dtype(self):
        return self._dtype

    @dtype.setter
    def dtype(self, val):
        if val not in DTYPES:
            raise ValueError('dtype not found')
        self._dtype = val
        self._bits, self._signed = DTYPES[val]
        self.emit('dtype_changed', self._dtype)

    @property
    def is_floating(self):
        return issubclass(self.dtype, (np.floating, bfloat16))

    @property
    def signed(self):
        return self._signed

    @signed.setter
    def signed(self, val):
        self._signed = val
        self._update_dtype()
        self.emit('signed_changed', self._dtype)

    @property
    def bits(self):
        return self._bits

    @bits.setter
    def bits(self, val):
        if self._bits == val:
            return
        self._bits = val
        if self._dtype is None:
            self._update_dtype()

        self.emit('bits_changed', self._dtype)
        if self.is_sq and self.min_val is not None and self.max_val is not None:
            self.recalculate_scale(
                self.min_val, self.max_val, self.narrow_range)

    @property
    def q(self):
        return self._q if self._q is not None else 0

    @q.setter
    def q(self, val):
        self._q = val
        self.emit('scale_changed', self._dtype)

    kInt8NearZeroTolerance = 1.0e-6

    @property
    def scale(self):
        if self._scale is None:
            if self._q is not None:
                #pylint: disable=invalid-unary-operand-type
                return self.init_array(math.pow(2, -self._q))
            return np.array([1.0], dtype=np.float32)
        #pylint: disable=invalid-unary-operand-type
        return self._scale * math.pow(2, -self.q)

    @scale.setter
    def scale(self, val):
        self._scale = np.atleast_1d(val)
        self.emit('scale_changed', self._dtype)

    @property
    def has_valid_range(self):
        return (self._min_val is not None and self._max_val is not None) or self._scale is not None

    @property
    def min_val(self):
        if self._min_val is None:
            return self.min
        return self._min_val

    @property
    def max_val(self):
        if self._max_val is None:
            return self.max
        return self._max_val

    @property
    def max_abs_val(self):
        if self._max_val is None or self._min_val is None:
            return self.max
        return max(abs(self._max_val), abs(self._min_val))

    @property
    def min(self):
        if issubclass(self.dtype, bfloat16):
            return np.array(-3.4e38)
        if issubclass(self.dtype, np.floating):
            finfo = np.finfo(self.dtype)
            return np.array(finfo.min)
        iinfo = np.iinfo(self.dtype)
        return (iinfo.min + (1 if self.narrow_range else 0) - self._zero_point) * self.scale

    @property
    def max(self):
        if issubclass(self.dtype, bfloat16):
            return np.array(3.4e38)
        if issubclass(self.dtype, np.floating):
            finfo = np.finfo(self.dtype)
            return np.array(finfo.max)
        iinfo = np.iinfo(self.dtype)
        return (iinfo.max - self._zero_point) * self.scale

    @property
    def narrow_range(self):
        return self._narrow_range

    @property
    def zero_point(self):
        return self._zero_point

    @property
    def quantized_dimension(self):
        return self._quantized_dimension

    @quantized_dimension.setter
    def quantized_dimension(self, val):
        self._quantized_dimension = val

    @property
    def ctype(self):
        return DTYPE_CTYPE[self.dtype]

    @property
    def length(self):
        return int(math.ceil(self._bits / 8))

    @property
    def range_val(self):
        if self.min_val is None and self.min_val is None:
            return None
        assert np.all(self.max_val >= self.min_val)
        return self.max_val - self.min_val

    @property
    def range(self):
        return self.max - self.min

    @staticmethod
    def init_array(val):
        return val if val is None else np.atleast_1d(val)

    @staticmethod
    def calculate_scale(rmin, rmax, qmin, qmax, dtype, asymmetric=False,
                        scale_zero_as_one=False, narrow_range=False,
                        zero_point=None):
        qrange = qmax - qmin
        if zero_point is not None:
            qpos_range = qmax - zero_point
            qneg_range = zero_point - qmin
            rpos_range = np.max(rmax, 0)
            rneg_range = np.min(rmin, 0)
            scale = np.maximum(
                divide_ignore(rpos_range, qpos_range),
                divide_ignore(rneg_range, qneg_range))
            return np.atleast_1d(scale), np.atleast_1d(zero_point)
        elif asymmetric:
            rrange = rmax - rmin
            scale = rrange / qrange
            zero_point_from_min = qmin - rmin / scale
            zero_point_from_max = qmax - rmax / scale
            zero_point_from_min_error = qmin + abs(rmin / scale)
            zero_point_from_max_error = qmax + abs(rmax / scale)
            # alternate code to do channel scaled zero point
            # if this is ever used the zero point adjustment code needs to
            # be modified
            # alt_zero_point = np.where(
            #     zero_point_from_min_error < zero_point_from_max_error,
            #     zero_point_from_min,
            #     zero_point_from_max
            # )
            # alt_nudged_zero_point = np.where(
            #     alt_zero_point < qmin,
            #     qmin,
            #     np.where(
            #         alt_zero_point > qmax,
            #         qmax,
            #         np.round(alt_zero_point)
            #     )
            # ).astype(np.int64)
            if len(zero_point_from_min_error) != 1:
                raise ValueError(
                    'asymmetric on dimension scaled tensors is not supported')
            if zero_point_from_min_error < zero_point_from_max_error:
                zero_point = zero_point_from_min
            else:
                zero_point = zero_point_from_max
            nudged_zero_point = 0
            if zero_point < qmin:
                nudged_zero_point = qmin
            elif zero_point > qmax:
                nudged_zero_point = qmax
            else:
                nudged_zero_point = math.floor(zero_point + 0.5)
            return np.atleast_1d(scale), np.atleast_1d(nudged_zero_point)
        else:
            rrange = QType.calculate_symmetric_real_range(
                rmin, rmax, dtype, narrow_range=narrow_range)
            if scale_zero_as_one:
                w_zero_cond = np.logical_and(rmin == rmax, rmax == 0)
                scale = np.where(w_zero_cond, 1, rrange / qrange)
            else:
                scale = rrange / qrange
            # cope with symmetric zeropoint of unsigned types
            if DTYPES[dtype][1]:
                zero_point = np.array([0], dtype=dtype)
            else:
                zero_point = np.atleast_1d((qmax + 1)//2)
            return np.atleast_1d(scale), zero_point

    def recalculate_scale(self, min_val, max_val, narrow_range=None, zero_point=None):
        if narrow_range is None:
            narrow_range = self.narrow_range
        qmin, qmax = self.calculate_quantized_range(
            self.bits, narrow_range=narrow_range, signed=self.signed)
        scale, zero_point = self.calculate_scale(
            min_val, max_val, qmin, qmax, self.dtype,
            asymmetric=self.is_asymmetric, narrow_range=narrow_range,
            zero_point=zero_point)
        self._zero_point = zero_point
        if np.any(scale != self.scale):
            self.scale = scale

    def scale_to_pow2(self):
        # closest above pow2 not channel scaled
        # if already just q set then return that
        if self._scale is None:
            return self
        # round up to nearest power of 2
        self._scale = np.power(2, np.ceil(np.log2(self._scale)))
        return self

    @staticmethod
    def reshape_transpose(trans, dim, val):
        return np.reshape(np.transpose(np.reshape(val, dim.shape), trans), val.shape)

    def reorder(self, trans, dim):
        if self._min_val is not None:
            if self._min_val.size <= 1:
                return
            min_val = self.reshape_transpose(trans, dim, self.min_val)
            max_val = self.reshape_transpose(trans, dim, self.max_val)
            self.recalculate_scale(
                min_val, max_val, narrow_range=self.narrow_range)
        else:
            if self.scale.size <= 1:
                return
            self.scale = self.reshape_transpose(trans, dim, self.scale)

    @classmethod
    def from_min_max(cls, min_val, max_val, dtype=None, bits=None, scaled=False,
                     asymmetric=False, narrow_range=False, quantized_dimension=None,
                     scale_zero_as_one=False, forced=False, zero_point=None, **kwargs):
        min_val = cls.init_array(min_val)
        max_val = cls.init_array(max_val)
        # check for scalar
        min_max_equal = np.isclose(min_val, max_val)
        #min_max_equal = min_val == max_val
        max_val = np.where(np.logical_and(
            min_max_equal, min_val < 0), -(min_val), max_val)
        min_val = np.where(np.logical_and(
            min_max_equal, min_val > 0), -(max_val), min_val)
        max_val = np.where(np.logical_and(
            min_max_equal, min_val == 0), 1, max_val)
        min_val = np.where(np.logical_and(
            min_max_equal, min_val == 0), 1, min_val)
        # zero must be representable
        min_val = np.where(min_val > 0, 0, min_val)
        max_val = np.where(max_val < 0, 0, max_val)
        # work out container
        if dtype is None:
            dtype = np.int8 if scaled else np.int16
        dtype_bits, signed = DTYPES[dtype]
        if bits is None:
            bits = dtype_bits
        elif bits > dtype_bits:
            raise ValueError(f'bits {bits} do not fit in dtype {dtype}')
        if scaled:
            qmin, qmax = cls.calculate_quantized_range(
                bits, narrow_range=narrow_range, signed=signed)
            scale, zero_point = cls.calculate_scale(
                min_val, max_val, qmin, qmax, dtype, asymmetric=asymmetric,
                scale_zero_as_one=scale_zero_as_one, narrow_range=narrow_range,
                zero_point=zero_point)
            if len(scale) == 1:
                quantized_dimension = None
            return cls(bits=bits, signed=signed, dtype=dtype, scale=scale,
                       zero_point=zero_point, quantized_dimension=quantized_dimension,
                       min_val=min_val, max_val=max_val, narrow_range=narrow_range,
                       forced=forced, **kwargs)
        else:
            if asymmetric:
                raise ValueError(
                    'asymmetric is not supported un unscaled mode')
            if quantized_dimension is not None:
                raise ValueError(
                    'quantized dimension is not supported un unscaled mode')
            int_bits = calc_bits(max_val, min_val, signed=signed)
            if int_bits + (1 if signed else 0) > bits:
                raise ValueError(
                    "number cannot be represented with this many bits")
            return cls(bits=bits, q=bits - int_bits, signed=signed, dtype=dtype,
                       min_val=min_val, max_val=max_val, narrow_range=narrow_range,
                       forced=forced, **kwargs)

    @classmethod
    def from_min_max_sq(cls, min_val, max_val, dtype=None, bits=None,
                        narrow_range=False, quantized_dimension=None,
                        scale_zero_as_one=False, asymmetric=False,
                        forced=False, zero_point=None, **kwargs):
        if dtype is None:
            dtype = np.int8
        return cls.from_min_max(min_val, max_val, dtype=dtype, bits=bits, scaled=True,
                                asymmetric=asymmetric, narrow_range=narrow_range,
                                quantized_dimension=quantized_dimension,
                                scale_zero_as_one=scale_zero_as_one,
                                forced=forced, zero_point=zero_point, **kwargs)

    @classmethod
    def from_min_max_pow2(cls, min_val, max_val, dtype=None, forced=False, **kwargs):
        return cls.from_min_max(min_val, max_val, dtype=dtype, scaled=False, forced=forced, **kwargs)

    @classmethod
    def from_array(cls, arr: np.ndarray, dtype=None, bits=None,
                   scaled=False, asymmetric=False, quantized_dimension=None,
                   narrow_range=False, scale_zero_as_one=False, forced=False, **kwargs):
        if quantized_dimension is not None:
            axes = tuple(axis for axis in range(len(arr.shape))
                         if axis != quantized_dimension)
        else:
            axes = None
        rmin = np.min(arr, axis=axes)
        rmin = np.where(np.abs(rmin) < 1e-8, 0, rmin)
        rmax = np.max(arr, axis=axes)
        rmax = np.where(np.abs(rmax) < 1e-8, 0, rmax)
        return cls.from_min_max(rmin, rmax, dtype=dtype, bits=bits, scaled=scaled,
                                asymmetric=asymmetric,
                                quantized_dimension=quantized_dimension,
                                narrow_range=narrow_range,
                                scale_zero_as_one=scale_zero_as_one,
                                forced=forced, **kwargs)

    @classmethod
    def from_array_sq(cls, arr: np.ndarray, dtype=None, bits=None,
                      quantized_dimension=None, narrow_range=False,
                      scale_zero_as_one=False, asymmetric=False,
                      forced=False, **kwargs):
        if dtype is None:
            dtype = np.int8
        return cls.from_array(arr, dtype=dtype, bits=bits, scaled=True, asymmetric=asymmetric,
                              quantized_dimension=quantized_dimension, narrow_range=narrow_range,
                              scale_zero_as_one=scale_zero_as_one, forced=forced, **kwargs)

    @classmethod
    def from_array_pow2(cls, arr: np.ndarray, dtype=None, bits=None, forced=False, **kwargs):
        return cls.from_array(arr, dtype=dtype, bits=bits, scaled=False, forced=forced, **kwargs)

    @staticmethod
    def calculate_quantized_range(bits, narrow_range=False, signed=True):
        if not signed:
            return (1 if narrow_range else 0), (int(math.pow(2, bits) - 1))
        act_bits = bits - 1 if signed else bits
        # max is one less than 2 ^ bits
        qmax = int(math.pow(2, act_bits) - 1)
        # if narrow range then max == abs(min)
        qmin = -(qmax + (0 if narrow_range else 1))
        return qmin, qmax

    @staticmethod
    def calculate_symmetric_real_range(rmin, rmax, dtype, narrow_range=False):
        if narrow_range:
            return np.maximum(rmax, np.abs(rmin)) * 2.0
        # cope with properly representing 0
        iinfo = np.iinfo(dtype)
        qrange = iinfo.max - iinfo.min
        mid = qrange // 2 + (1 if qrange % 2 else 0)
        qmin = -mid
        qmax = qrange - mid
        # max offset by the min asymmetric amount
        max_clipped = np.abs(rmax) * qmax / np.abs(qmin)
        min_is_max = np.greater(np.abs(rmin), max_clipped)
        return np.where(
            min_is_max,
            # range from clipped max to min
            np.abs(rmin) * qmax / np.abs(qmin) - rmin,
            # range from max to expanded max
            rmax + rmax * np.abs(qmin) / qmax
        )

    def clip(self, arr: np.ndarray, dtype=None, bits=None, narrow_range=False):
        if dtype is None:
            dtype = self._dtype
        if bits is None:
            bits = self._bits
        if narrow_range is None:
            narrow_range = self._narrow_range
        qmin, qmax = self.calculate_quantized_range(
            bits, narrow_range=narrow_range, signed=self._signed)
        return np.minimum(np.maximum(arr, qmin), qmax).astype(dtype)

    def quantize(self, arr):
        if self.is_floating or self.scale is None:
            return arr.astype(self.dtype)
        if self.quantized_dimension is not None:
            bshape = [dim if self._quantized_dimension ==
                      idx else 1 for idx, dim in enumerate(arr.shape)]
            scale = self.scale if len(
                self.scale) == 1 else np.reshape(self.scale, bshape)
            zero_point = self.zero_point if len(
                self.zero_point) == 1 else np.reshape(self.zero_point, bshape)
            arr = np.floor((arr / scale) + 0.5) + zero_point
        else:
            arr = np.floor((arr / self.scale) + 0.5) + self.zero_point
        if self._offset is not None:
            arr += self._offset
        return self.clip(arr).astype(self.dtype)

    def dequantize(self, arr):
        if self.is_floating or self.scale is None:
            return arr.astype(np.float32)
        if self._offset is not None:
            arr = arr - self._offset
        if self.quantized_dimension is not None:
            bshape = [dim if self._quantized_dimension ==
                      idx else 1 for idx, dim in enumerate(arr.shape)]
            scale = self.scale if len(
                self.scale) == 1 else np.reshape(self.scale, bshape)
            zero_point = self.zero_point if len(
                self.zero_point) == 1 else np.reshape(self.zero_point, bshape)
        else:
            scale = self.scale
            zero_point = self.zero_point
        return (arr.astype(np.float64) - zero_point) * scale

    def quantize_from(self, arr, qtype):
        return self.quantize(qtype.dequantize(arr))

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

    def expand_from(self, arr, other, allow_zero_adjust=False):
        if self.q == other.q:
            if not allow_zero_adjust and self.zero_point != other.zero_point:
                raise ValueError(
                    'expansion step is adjusting zero point which will result in an incorrect result')
            return arr.astype(self.dtype)
        return self.expand_normalize(arr, other)

    def reduce_from(self, arr, other, allow_zero_adjust=False):
        if self.q == other.q:
            if not allow_zero_adjust and self.zero_point != other.zero_point:
                raise ValueError(
                    'reduction step is adjusting zero point which will result in an incorrect result')
            return self.clip(arr)
        return self.round_normalize_clip(arr, other)

    def get_quantized_scale(self):
        scales, norms = compute_mul_bias(self.scale)
        return scales.astype(np.uint8), norms, self.zero_point

    def __mul__(self, other):
        # mul handles the case of the qtype result from multiplying values in the two qtypes
        # This does not handle differing zero points
        assert self.zero_point == 0 and other.zero_point == 0
        if other.is_pow2 and self.is_pow2:
            return QType(q=self.q + other.q, bits=self.bits + other.bits, signed=self.signed or other.signed)
        return QType(scale=self.scale * other.scale, bits=self.bits + other.bits, signed=self.signed or other.signed)

    def quantization_equal(self, other):
        return (np.allclose(self.scale, other.scale) and
                self.bits == other.bits and
                self.signed == other.signed and
                self.dtype == other.dtype and
                self.quantized_dimension == other.quantized_dimension and
                np.allclose(self.zero_point, other.zero_point) and
                np.all(self.offset == other.offset))

    def __eq__(self, other):
        if isinstance(other, QType):
            return (self.quantization_equal(other) and
                    self.attr == other.attr)
        return other.__eq__(self)

    @staticmethod
    def str_fmt(val, extend=False):
        if val is None:
            return "unkn"
        if isinstance(val, (int, float)) or isinstance(val, (np.floating, np.integer)):
            return val
        return "chan" if len(val) > 1 else ("{:0.2f}".format(val[0]) if not extend else "{:0.8f}".format(val[0]))

    @property
    def dtype_str(self):
        if self.bits < 8:
            if self.dtype == np.int8:
                return f'i{self.bits}'
            elif self.dtype == np.uint8:
                return f'u{self.bits}'
        return DTYPE_STR[self.dtype]

    def str_by_chan(self, chan: int):
        return "{}<({}-{})*{}<{}".format(
            self.str_fmt(self.min[chan]),
            self.dtype_str,
            self.str_fmt(self.zero_point[chan] if chan < len(
                self.zero_point) else self.zero_point[0]),
            self.str_fmt(self.scale[chan]),
            self.str_fmt(self.max[chan]),
        )

    def __str__(self):
        if issubclass(self.dtype, (np.floating, bfloat16)):
            return f'{self.dtype_str}'
        if self.is_sq:
            return (f"{self.str_fmt(self.min)}<({self.dtype_str}-{self.str_fmt(self.zero_point)})"
                    f"*{self.str_fmt(self.scale, extend=True)}<{self.str_fmt(self.max)}"
                    f"{' forced' if self.forced else ''}")
        if self.q > self.bits:
            return f"M{self.bits}>>{self.q}{' forced' if self.forced else ''}"
        return f"Q{self.bits - self.q}.{self.q}{' forced' if self.forced else ''}"

    def __repr__(self) -> str:
        if issubclass(self.dtype, (np.floating, bfloat16)):
            return f'QType({self.dtype_str})'
        return 'QType('+", ".join(f'{k}={getattr(self, k)}' for k in self.EXPORT if getattr(self, k) is not None)+')'
