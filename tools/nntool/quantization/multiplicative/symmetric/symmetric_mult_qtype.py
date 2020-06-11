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

from quantization.multiplicative.mult_qtype_base import MultQTypeBase

VALID_DTYPES = [
    np.int8,
    np.int16,
    np.int32
]


class SymmetricMultQType(MultQTypeBase):
    def __init__(self, *args, narrow_range=False, init=None, **kwargs):
        super(SymmetricMultQType, self).__init__(*args, init=init, **kwargs)
        if init is None:
            self._info['narrow_range'] = narrow_range

    SYMMETRIC_UINT = {
        np.uint8: (128, np.int8)
    }

    def dtype_is_valid(self):
        return self.dtype in VALID_DTYPES

    def verify(self, arr, dimension):
        """Verify that any 0 filters have a 1 scale"""
        dq_arr = self.dequantize(arr)
        axes = tuple(axis for axis in range(len(dq_arr.shape)) if axis != dimension)
        rmin = np.min(dq_arr, axis=axes)
        rmax = np.min(dq_arr, axis=axes)
        self.scale = np.where((rmin == 0) & (rmax == 0), 1, self.scale)

    @classmethod
    def from_tflite(cls, tf_qps, dtype):
        res = cls()
        res.min_val = tf_qps.MinAsNumpy() if tf_qps.MinLength() > 0 else None
        res.max_val = tf_qps.MaxAsNumpy() if tf_qps.MaxLength() > 0 else None
        if res.min_val is not None and res.max_val is not None \
                                        and np.all(np.abs(res.min_val) == res.max_val):
            res.narrow_range = True
        res.scale = tf_qps.ScaleAsNumpy() if tf_qps.ScaleLength() > 0 else None
        res.quantized_dimension = tf_qps.QuantizedDimension()
        if dtype in cls.SYMMETRIC_UINT:
            zero_point, signed_dtype = cls.SYMMETRIC_UINT[dtype]
            assert np.all(tf_qps.ZeroPointAsNumpy() == zero_point)
            res.dtype = signed_dtype
        else:
            res.dtype = dtype
        return res

    @classmethod
    def from_array(cls, arr: np.ndarray, dtype=np.int8,
                   quantized_dimension=None, narrow_range=False):

        if quantized_dimension is not None:
            axes = tuple(axis for axis in range(len(arr.shape)) if axis != quantized_dimension)
        else:
            axes = None
        rmin = np.min(arr, axis=axes)
        rmax = np.max(arr, axis=axes)
        return cls.from_min_max(rmin, rmax, dtype=dtype,
                                quantized_dimension=quantized_dimension,
                                narrow_range=narrow_range)

    @classmethod
    def from_min_max(cls, min_val, max_val, dtype=np.int8, quantized_dimension=None, narrow_range=False):
        val = cls(min_val=min_val, max_val=max_val,
                  quantized_dimension=quantized_dimension, dtype=dtype,
                  narrow_range=narrow_range)
        iinfo = np.iinfo(dtype)

        if narrow_range:
            ranges = iinfo.max - (iinfo.min + 1)
        else:
            ranges = iinfo.max - iinfo.min
        val.scale = val.range / ranges
        return val

    def scale_to_pow2(self):
        # closest above pow2
        self.scale = 2**np.ceil(np.log2(self.scale))

    def _encapsulate(self):
        return {
            "min_val": self.min,
            "max_val": self.max,
            "scale": self.scale,
            "dim": self.quantized_dimension,
            "narrow_range": self.narrow_range,
            "dtype": self.dtype.__name__
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init={
            "min_val": val['min_val'],
            "max_val": val['max_val'],
            "scale": val['scale'],
            "quantized_dimension": val['dim'] if 'dim' in val else None,
            "narrow_range": val['narrow_range'],
            "dtype": getattr(np, val['dtype'])
        })

    @property
    def narrow_range(self):
        return self._info.get('narrow_range')

    @narrow_range.setter
    def narrow_range(self, val):
        self._info['narrow_range'] = val

    @property
    def min(self):
        if self.max is None:
            return None
        if (not self.narrow_range) and (self.max_val != self.min_val).all():
            dtype_info = np.iinfo(self.dtype)
            max_calc = - np.abs(self.max_val) * np.abs(dtype_info.min) / dtype_info.max
            min_val_is_min = np.less_equal(self.max_val * dtype_info.max / np.abs(dtype_info.min), np.abs(self.min_val))
            #min_val_is_min = np.less(self.max_val, np.abs(self.min_val))
            max_calc = np.where(min_val_is_min, self.min_val, max_calc)
            return max_calc
        return self.max * -1

    @property
    def max(self):
        if self.min_val is None or self.max_val is None:
            return None
        if (not self.narrow_range) and (self.max_val != self.min_val).all():
            dtype_info = np.iinfo(self.dtype)
            max_calc = np.abs(self.min_val) * dtype_info.max / np.abs(dtype_info.min)
            max_val_is_max = np.greater(self.max_val * dtype_info.max / np.abs(dtype_info.min), np.abs(self.min_val))
            #max_val_is_max = np.greater_equal(self.max_val, np.abs(self.min_val))
            max_calc = np.where(max_val_is_max, self.max_val, max_calc)
            return max_calc

        max_calc = np.maximum(np.abs(self.min_val), np.abs(self.max_val))
        max_calc[max_calc == 0] = 1
        return max_calc

    @property
    def zero_point(self):
        return np.array([0])

    @zero_point.setter
    def zero_point(self, val):
        raise ValueError()

    def get_quantized_scale(self):
        max_val = math.pow(2, 8)
        factors = np.array([math.frexp(scale) for scale in self.scale],
                           dtype=[("scale", "f4"), ("norm", "i1")])
        qscales = np.floor(factors['scale'] * max_val + 0.5)
        qnorms = -factors["norm"]
        overflow = qscales >= max_val
        qnorms[overflow] -= 1
        qscales = np.where(overflow, qscales // 2, qscales)
        return qscales.astype(np.uint8), qnorms

    def dequantize_at_scale(self, arr: np.array, scale: np.array) -> np.array:
        arr = arr.astype(np.float32)
        if len(scale) > 1 and arr.shape != scale.shape:
            return arr * scale.reshape(tuple(size if idx == self.quantized_dimension else 1
                                             for idx, size in enumerate(arr.shape)))
        return arr * scale

    def quantize_at_scale(self, arr: np.array, scale: np.array) -> np.array:
        if len(scale) > 1 and arr.shape != scale.shape:
            arr = np.floor(0.5 + arr * 1 / scale.reshape(tuple(size if idx == self.quantized_dimension else 1
                                                               for idx, size in enumerate(arr.shape))))
        else:
            arr = np.floor(arr/scale + 0.5)
        return self.clip(arr, narrow_range=self.narrow_range)

    def quantize(self, arr: np.array) -> np.array:
        return self.quantize_at_scale(arr, self.scale)

    def dequantize(self, arr: np.array) -> np.array:
        return self.dequantize_at_scale(arr, self.scale)

    def get_dequantized(self, arr, container_is_quantized=True):
        if container_is_quantized:
            return self.dequantize_at_scale(arr, self.scale)
        return arr

    def get_quantized(self, arr: np.array, container_is_quantized=True) -> np.array:
        if not container_is_quantized:
            return self.quantize_at_scale(arr, self.scale)
        return arr

    def str_by_chan(self, chan: int):
        if self.min is None and self.max is None:
            return "{}*{}".format(
                self.dtype_str,
                self.str_fmt(self.scale[chan], extend=True),
            )
        return "{}<{}*{}<{}".format(
            self.str_fmt(self.min[chan]),
            self.dtype_str,
            self.str_fmt(self.scale[chan], extend=True),
            self.str_fmt(self.max[chan]),
        )

    def __str__(self):
        if self.min is None and self.max is None:
            return "{}*{}".format(
                self.dtype_str,
                self.str_fmt(self.scale, extend=True),
            )
        return "{}<{}*{}<{}".format(
            self.str_fmt(self.min),
            self.dtype_str,
            self.str_fmt(self.scale, extend=True),
            self.str_fmt(self.max),
        )
