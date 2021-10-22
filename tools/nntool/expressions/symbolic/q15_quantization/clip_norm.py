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
from expressions.symbolic.basic import Add

from ..function import Function
from ..symbol import c_headers, nargs


@nargs(1)
@c_headers('"Gap.h"')
class Clip(Function):
    def __init__(self, *args, clip_dtype=None, **kwargs):
        iinfo = np.iinfo(clip_dtype)
        self._clip_max = iinfo.max
        self._clip_min = iinfo.min
        super().__init__(*args, **kwargs)

    def _impl(self, *args, **kwargs):
        return np.maximum(np.minimum(args[0], self._clip_max, dtype=self.dtype), self._clip_min, dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.maximum(np.minimum(%s, %s), %s)" % (args[0], self._clip_max, self._clip_min)

    def _c_expr(self, *args, **kwargs):
        if np.issubdtype(self.dtype, np.signedinteger):
            return "gap_clip((%s), (%s))" % (args[0], math.ceil(math.log2(self._clip_max)))
        return "gap_clipu((%s), (%s))" % (args[0], math.ceil(math.log2(self._clip_max)))

    def __repr__(self) -> str:
        return "Clip(%s, %s, %s)" % (self.contents[0], self._clip_max, self._clip_min)


@nargs(2)
@c_headers('"Gap.h"')
class Norm(Function):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def _impl(self, *args, **kwargs):
        dtype = self.dtype
        round_amount = np.where(args[1] == 0, 0, (dtype(
            1) << (args[1].astype(dtype) - dtype(1))))
        return (np.add(args[0], round_amount, dtype=dtype) >> args[1]).astype(dtype)

    def _py_expr(self, *args, **kwargs):
        # this is not correct since it does not take into account unsigned but not used at present
        return f"(({args[0]} + np.where({args[0]} != 0, (1 << ({args[1]}.astype(np.int32) - 1)), 0)) >> {args[0]}.astype(np.int32))"

    def _c_expr(self, *args, **kwargs):
        try:
            num = int(args[1])
            if num:
                if np.issubdtype(self.dtype, np.signedinteger):
                    return f"gap_roundnorm({args[0]}, {num})"
                return f"gap_roundnormu({args[0]}, {num})"
            return args[0]
        except (TypeError, ValueError):
            if np.issubdtype(self.dtype, np.signedinteger):
                return f"gap_roundnorm_reg({args[0]}, {args[1]})"
            return f"gap_roundnormu_reg({args[0]}, {args[1]})"

    def _eval(self, *args, **kwargs):
        sym = args[0]
        # if the argument is another scalequantized do the scaling in one step
        # this should be safe as we never go much above Q15 and the scaling step
        # is also a Q15
        if not isinstance(sym, Norm):
            return super()._eval(*args, **kwargs)
        sym.contents[1] = Add(sym.contents[1], args[1], dtype=np.int32)
        return sym
