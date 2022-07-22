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
from copy import deepcopy

import numpy as np

from nntool.quantization.qtype import QType

from ..quantization_base import QRecBase

OUT_Q = {
    np.uint8: 8,
    np.int8: 7,
    np.uint16: 16,
    np.int16: 15,
}

class Q15ScaleQRec(QRecBase):
    def __init__(self, dtype: np.dtype, scale: float, q: int, min_val=None, max_val=None, zero_point=0) -> None:
        super(Q15ScaleQRec, self).__init__(dtype)
        if isinstance(scale, np.ndarray):
            scale = scale.item()
        self._scale = scale
        self._q = q
        self._min_val = min_val
        self._max_val = max_val
        self._zero_point = zero_point

    @property
    def scaledq(self):
        return self.scale * math.pow(2, -self.q)

    @classmethod
    def from_qtype(cls, qtype: QType):
        # convert between external QType and internal expression QType. Why are their 2
        # - legacy issues. Should be refactored to have one.
        # Internal to expression in fixed point we monitor the Q and scale separately
        q = OUT_Q[qtype.dtype]
        scale = qtype.scale * pow(2, q)
        return Q15ScaleQRec(
            qtype.dtype,
            scale,
            q,
            min_val=qtype.min_val,
            max_val=qtype.max_val,
            zero_point=qtype.zero_point)

    @classmethod
    def from_range(cls, dtype: np.dtype, q: int, min_val: float, max_val:float):
        scale_min = abs(min_val) / pow(2, q)
        scale_max = abs(max_val) / (pow(2, q) - 1)
        if scale_min > scale_max:
            scale = np.abs(min_val)
        else:
            scale = max_val * pow(2, 7) / (pow(2, 7) - 1)
        return Q15ScaleQRec(dtype, scale, q, min_val, max_val)


    @classmethod
    def inherit(cls, rec, dtype: np.dtype = None, scale: float = None, q: int = None, max_val=None, min_val=None, zero_point=None):
        if rec is None:
            return cls(dtype, scale, q, max_val=max_val, min_val=min_val, zero_point=zero_point)
        if rec.q is None:
            rec.q = q
        if rec.dtype is None:
            rec.dtype = dtype
        if rec.scale is None:
            rec.scale = scale
        if rec.zero_point is None:
            rec.zero_point = zero_point
        return rec

    @classmethod
    def override(cls, rec, dtype: np.dtype = None, scale: float = None, q: int = None, zero_point=None):
        rec = deepcopy(rec)
        if q is not None:
            rec.q = q
        if dtype is not None:
            rec.dtype = dtype
        if scale is not None:
            rec.scale = scale
        if zero_point is not None:
            rec.zero_point = zero_point
        return rec

    @classmethod
    def compute_scales(cls, scale, available_bits=None):
        fac, norm = math.frexp(scale)
        # if fac is .5 then this is just a shift
        if fac == 0.5:
            return 1, -(norm-1)
        fac = math.floor(fac * math.pow(2, available_bits) + 0.5)
        # rshift is positive
        return fac, -(norm-available_bits)

    @classmethod
    def quantized_scale(cls, from_qrec, to_qrec, bits, dtype=np.int32):
        scale = from_qrec.scaledq/to_qrec.scaledq
        return cls.compute_scales(scale, available_bits=bits)

    @property
    def min_val(self):
        return self._min_val

    @property
    def max_val(self):
        return self._max_val

    @property
    def max_abs(self):
        if any(val is None for val in (self._max_val, self._min_val)):
            return None
        return np.max(np.abs([self._max_val, self._min_val]))

    @property
    def zero_point(self):
        return self._zero_point

    @zero_point.setter
    def zero_point(self, val):
        self._zero_point = val


    @property
    def scale(self):
        return self._scale

    @scale.setter
    def scale(self, val):
        self._scale = val

    @property
    def q(self):
        return self._q

    @q.setter
    def q(self, val):
        self._q = val

    def quantize(self, val):
        if np.all(val == 0):
            return val
        return (np.round(np.power(2.0, self.q) * val/self.scale, 0) + self.zero_point).astype(self.dtype)

    def quantize_and_clip(self, val):
        if np.all(val == 0):
            return val.astype(self.dtype)
        iinfo = np.iinfo(self.dtype)
        return np.clip((np.round(np.power(2.0, self.q) * val/self.scale, 0) + self.zero_point), iinfo.min, iinfo.max).astype(self.dtype)

    def quantize_py_expr(self, val):
        if np.all(val == 0):
            return f"{val}.astype(np.{self.dtype.__name__})"
        return f"np.round((np.power(2, {self.q}) * ({val})/{self.scale}, 0) + {self.zero_point}).astype(np.{self.dtype.__name__})"

    def quantize_and_clip_py_expr(self, val):
        if np.all(val == 0):
            return f"{val}.astype(np.{self.dtype.__name__})"
        iinfo = np.iinfo(self.dtype)
        return f"np.clip((np.round(np.power(2, {self.q}) * ({val})/{self.scale}, 0) + {self.zero_point}), {iinfo.min}, {iinfo.max}).astype(np.{self.dtype.__name__})"

    def quantize_c_expr(self, val):
        return "((%s)round(2^(%s) * (%s)/(%s), 0))" % (self.ctype, self.q, val, self.scale)

    def quantize_and_clip_c_expr(self, val):
        iinfo = np.iinfo(self.dtype)
        bits = math.ceil(math.log2(iinfo.max))
        return "gap_clip(((%s)round(2^(%s) * (%s)/(%s), 0)), %s)" % (self.ctype, self.q, val, self.scale, bits)

    def dequantize(self, val):
        return (val.astype(np.float) - self.zero_point) * self.scale * math.pow(2, -self.q)

    def dequantize_py_expr(self, val):
        return "%s.astype(np.float) * %s * math.pow(2, -%s)" % (val, self.scale, self.q)

    def dequantize_c_expr(self, val):
        return "((float)(%s))*(%s)*(2^(-%s))" % (val, self.scale, self.q)

    def __str__(self) -> str:
        return self.__repr__()

    def __eq__(self, o: object) -> bool:
        return self.q == o.q and self.scale == o.scale and self.dtype == o.dtype and self.zero_point == o.zero_point

    def __repr__(self) -> str:
        return f"{self._dtype.__name__} {self.scale:.3f} Q{self._q}"
