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

from ..quantization_base import QRecBase


class Q15ScaleQRec(QRecBase):
    def __init__(self, dtype: np.dtype, scale: float, q: int, min_val=None, max_val=None) -> None:
        self._dtype = dtype
        self._scale = scale
        self._q = q
        self._min_val = min_val
        self._max_val = max_val

    def __repr__(self) -> str:
        return f"{self._dtype.__name__} {self.scale} Q{self._q}"

    @classmethod
    def inherit(cls, rec, dtype: np.dtype = None, scale: float = None, q: int = None, max_val=None, min_val=None):
        if rec is None:
            return cls(dtype, scale, q, max_val=max_val, min_val=min_val)
        if rec.q is None:
            rec.q = q
        if rec.dtype is None:
            rec.dtype = dtype
        if rec.scale is None:
            rec.scale = scale
        return rec

    @classmethod
    def override(cls, rec, dtype: np.dtype = None, scale: float = None, q: int = None):
        rec = deepcopy(rec)
        if q is not None:
            rec.q = q
        if dtype is not None:
            rec.dtype = dtype
        if scale is not None:
            rec.scale = scale
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
    def dtype(self):
        return self._dtype

    @property
    def ctype(self):
        return self.DTYPE_TO_CTYPE[self.dtype]

    @dtype.setter
    def dtype(self, val):
        self._dtype = val

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
        return np.round(np.power(2, self.q) * val/self.scale, 0).astype(self.dtype)

    def quantize_and_clip(self, val):
        if np.all(val == 0):
            return val.astype(self.dtype)
        iinfo = np.iinfo(self.dtype)
        return np.clip(np.round(np.power(2, self.q) * val/self.scale, 0), iinfo.min, iinfo.max).astype(self.dtype)

    def quantize_py_expr(self, val):
        if np.all(val == 0):
            return f"{val}.astype(np.{self.dtype.__name__})"
        return f"np.round(np.power(2, {self.q}) * ({val})/{self.scale}, 0).astype(np.{self.dtype.__name__})"

    def quantize_and_clip_py_expr(self, val):
        if np.all(val == 0):
            return f"{val}.astype(np.{self.dtype.__name__})"
        iinfo = np.iinfo(self.dtype)
        return f"np.clip(np.round(np.power(2, {self.q}) * ({val})/{self.scale}, 0), {iinfo.min}, {iinfo.max}).astype(np.{self.dtype.__name__})"

    def quantize_c_expr(self, val):
        return "((%s)round(2^(%s) * (%s)/(%s), 0))" % (self.ctype, self.q, val, self.scale)

    def quantize_and_clip_c_expr(self, val):
        iinfo = np.iinfo(self.dtype)
        bits = math.ceil(math.log2(iinfo.max))
        return "gap_clip(((%s)round(2^(%s) * (%s)/(%s), 0)), %s)" % (self.ctype, self.q, val, self.scale, bits)

    def dequantize(self, val):
        return val.astype(np.float) * self.scale * math.pow(2, -self.q)

    def dequantize_py_expr(self, val):
        return "%s.astype(np.float) * %s * math.pow(2, -%s)" % (val, self.scale, self.q)

    def dequantize_c_expr(self, val):
        return "((float)(%s))*(%s)*(2^(-%s))" % (val, self.scale, self.q)

    @property
    def scaledq(self):
        return self.scale * math.pow(2, -self.q)

    def __str__(self) -> str:
        return self.__repr__()

    def __eq__(self, o: object) -> bool:
        return self.q == o.q and self.scale == o.scale
