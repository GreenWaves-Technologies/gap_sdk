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

import numpy as np

from ..quantization_base import QRecBase


class FloatQRec(QRecBase):
    def __init__(self, dtype: np.dtype, min_val=None, max_val=None) -> None:
        self._dtype = dtype
        self._min_val = min_val
        self._max_val = max_val

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

    def quantize_py_expr(self, val):
        return f"np.astype(np.{self.dtype.__name__})"

    @classmethod
    def override(cls, rec, dtype: np.dtype = None):
        rec = deepcopy(rec)
        if dtype is not None:
            rec.dtype = dtype
        return rec
