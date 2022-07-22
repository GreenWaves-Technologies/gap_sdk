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

from copy import deepcopy
from bfloat16 import bfloat16

import numpy as np

from ..quantization_base import QRecBase

DTYPE_NAMES = {
    np.float64: 'f64',
    np.float32: 'f32',
    np.float16: 'f16',
    bfloat16: 'bf16'
}

class FloatQRec(QRecBase):
    def __init__(self, dtype: np.dtype, min_val=None, max_val=None) -> None:
        super(FloatQRec, self).__init__(dtype)
        self._min_val = min_val
        self._max_val = max_val

    def __repr__(self) -> str:
        return DTYPE_NAMES[self._dtype]

    @property
    def min_val(self):
        return self._min_val

    @property
    def max_val(self):
        return self._max_val

    def quantize_py_expr(self, val):
        return f"np.astype(np.{self.dtype.__name__})"

    def quantize(self, val):
        return val.astype(self.dtype)

    def dequantize(self, val):
        return val.astype(np.float32)

    @classmethod
    def override(cls, rec, dtype: np.dtype = None):
        rec = deepcopy(rec)
        if dtype is not None:
            rec.dtype = dtype
        return rec
