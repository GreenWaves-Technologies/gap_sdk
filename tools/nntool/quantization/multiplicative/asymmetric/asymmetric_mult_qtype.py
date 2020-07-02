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
    np.int32,
    np.uint8,
    np.uint16,
    np.uint32
]

SIGNED_DTYPES = [
    np.int8,
    np.int16,
    np.int32,
]
# template <ArrayDataType A>
# QuantizationPoints GetQuantizationPoints() {
#   QuantizationPoints qp;
#   using Integer = DataType<A>;
#   qp.min_value = std::numeric_limits<Integer>::min();
#   qp.max_value = std::numeric_limits<Integer>::max();
#   // eg [-128,127]...
#   qp.central_value = (qp.min_value / 2 +        // -128 -> -64.
#                       (qp.max_value - 1) / 2 +  // 127 -> 63.
#                       1);
#   return qp;
# }


class AsymmetricMultQType(MultQTypeBase):
    def __init__(self, *args, zero_point=0, scale=None, init=None, **kwargs):
        super(AsymmetricMultQType, self).__init__(*args, init=init, **kwargs)
        if init is None:
            self.zero_point = self.init_array(zero_point)
            self.scale = self.init_array(scale)

    @classmethod
    def from_tflite(cls, tf_qps, dtype):
        res = cls(zero_point=tf_qps.ZeroPointAsNumpy() if tf_qps.ZeroPointLength() > 0 else None)
        res.min_val = tf_qps.MinAsNumpy() if tf_qps.MinLength() > 0 else None
        res.max_val = tf_qps.MaxAsNumpy() if tf_qps.MaxLength() > 0 else None
        res.scale = tf_qps.ScaleAsNumpy() if tf_qps.ScaleLength() > 0 else None
        res.zero_point = tf_qps.ZeroPointAsNumpy() if tf_qps.ZeroPointLength() > 0 else None
        res.quantized_dimension = tf_qps.QuantizedDimension()
        res.dtype = dtype
        return res

    @classmethod
    def from_array(cls, arr: np.ndarray, dtype=np.uint8,
                   quantized_dimension=None, narrow_range=False):
        rmin = np.min(arr)
        rmax = np.max(arr)
        return cls.from_min_max(rmin, rmax, dtype=dtype, quantized_dimension=quantized_dimension,
                                narrow_range=narrow_range)

    @classmethod
    def from_min_max(cls, rmin, rmax, dtype=np.uint8,
                     quantized_dimension=None, narrow_range=False):
        iinfo = np.iinfo(dtype)
        qmin = iinfo.min + (1 if narrow_range else 0)
        qmax = iinfo.max
        if rmin == rmax:
            if rmin == 0:
                return cls(min_val=0, max_val=0, scale=0, zero_point=0,
                           quantized_dimension=quantized_dimension, dtype=dtype)
            elif rmin < 0:
                rmax = -rmin
            else:
                rmin = -rmax
        # we must represent 0
        if rmin > 0:
            rmin = 0
        scale = (rmax - rmin) / (qmax - qmin)
        zero_point_from_min = qmin - rmin / scale
        zero_point_from_max = qmax - rmax / scale
        zero_point_from_min_error = qmin + abs(rmin / scale)
        zero_point_from_max_error = qmax + abs(rmax / scale)
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

        return cls(min_val=rmin, max_val=rmax, scale=scale, zero_point=nudged_zero_point,
                   quantized_dimension=quantized_dimension, dtype=dtype)

    @property
    def central(self):
        iinfo = np.iinfo(self.dtype)
        if iinfo.min == 0:
            return iinfo.min // 2 + (iinfo.max - 1) // 2 + 1
        return 0

    @property
    def min(self):
        return self.min_val

    @property
    def max(self):
        return self.max_val

    def get_quantized_scale(self):
        return [0], [0]

    def dtype_is_valid(self):
        return self.dtype in VALID_DTYPES

    def _encapsulate(self):
        return {
            "min_val": self.min_val,
            "max_val": self.max_val,
            "scale": self.scale,
            "zero_point": self.zero_point,
            "dim": self.quantized_dimension,
            "dtype": self.dtype.__name__
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init={
            "min_val": val['min_val'],
            "max_val": val['max_val'],
            "scale": val['scale'],
            "zero_point": val['zero_point'],
            "quantized_dimension": val['dim'] if 'dim' in val else None,
            "dtype": getattr(np, val['dtype'])
        })

    @property
    def pad_zero_point(self):
        return self.zero_point[0]

    @property
    def zero_point(self):
        return self._info['zero_point']

    @zero_point.setter
    def zero_point(self, val):
        self._info['zero_point'] = val

    def quantize(self, arr: np.array) -> np.array:
        arr = np.floor(arr/self.scale + 0.5) + self.zero_point
        return self.clip(arr)

    def dequantize(self, arr: np.array) -> np.array:

        shape = tuple(dim if idx == self.quantized_dimension
                      else 1 for idx, dim in enumerate(arr.shape))
        if len(self.zero_point) > 1:
            zero_point = self.zero_point.reshape(shape)
        else:
            zero_point = self.zero_point

        if len(self.scale) > 1:
            scale = self.scale.reshape(shape)
        else:
            scale = self.scale

        arr = (arr.astype(np.float32) - zero_point) * scale
        return arr

    def get_dequantized(self, arr, container_is_quantized=True):
        if container_is_quantized:
            return self.dequantize(arr)
        return arr

    def get_quantized(self, arr: np.array, container_is_quantized=True) -> np.array:
        if not container_is_quantized:
            return self.quantize(arr)
        return arr
