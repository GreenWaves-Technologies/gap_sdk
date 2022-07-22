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


from typing import Tuple

import numpy as np
from nntool.expressions.symbolic.common.common_quantizer import FloatQ15QuantizationBase

from ..quantization_base import handles_scheme
from ..symbol import Symbol, SymbolStats
from .q15_scale_q_rec import Q15ScaleQRec


@handles_scheme('Q15Scale')
class Q15ScaledQuantization(FloatQ15QuantizationBase):
    MAXQ_FROM_DTYPE = {
        np.int32: 30,
        np.int16: 15,
        np.int8: 7,
    }

    @classmethod
    def get_maxq_from_dtype(cls, dtype: np.dtype):
        return cls.MAXQ_FROM_DTYPE[dtype]

# pylint disable=arguments-differ
    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:
        return super(Q15ScaledQuantization, cls)._quantize(sym, sym_ctrl, qrec, **kwargs)

# pylint disable=arguments-differ
    @classmethod
    def _dequantize(cls, val, qrec: Q15ScaleQRec, **kwargs) -> np.ndarray:
        return qrec.dequantize(val)

# pylint disable=arguments-differ
    @classmethod
    def _dequantize_py_expr(cls, py_expr: str, qrec: Q15ScaleQRec, **kwargs) -> np.ndarray:
        return qrec.dequantize_py_expr(py_expr)

# pylint disable=arguments-differ
    @classmethod
    def _dequantize_c_expr(cls, c_expr: str, qrec: Q15ScaleQRec, **kwargs) -> np.ndarray:
        return qrec.dequantize_c_expr(c_expr)
