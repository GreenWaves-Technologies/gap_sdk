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
from typing import Tuple

import numpy as np

from ..basic import Cast, ConvertFloatScaled
from ..q15_quantization.q15_scale_q_rec import Q15ScaleQRec
from ..quantization_base import (QRecBase, QuantizationHandlerBase,
                                 handles_scheme)
from ..symbol import Symbol, SymbolStats
from .float_qrec import FloatQRec


@handles_scheme('Float')
class FloatQuantization(QuantizationHandlerBase):

# pylint disable=arguments-differ
    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:
        return super(FloatQuantization, cls)._quantize(sym, sym_ctrl, qrec, **kwargs)

# pylint disable=arguments-differ
    @classmethod
    def _dequantize(cls, val, qrec: FloatQRec, **kwargs) -> np.ndarray:
        return qrec.dequantize(val)

# pylint disable=arguments-differ
    @classmethod
    def _dequantize_py_expr(cls, py_expr: str, qrec: FloatQRec, **kwargs) -> np.ndarray:
        return qrec.dequantize_py_expr(py_expr)

# pylint disable=arguments-differ
    @classmethod
    def _dequantize_c_expr(cls, c_expr: str, qrec: FloatQRec, **kwargs) -> np.ndarray:
        return qrec.dequantize_c_expr(c_expr)

    @classmethod
    def _quantize_output(cls,
                         sym: Symbol,
                         qsym: Symbol,
                         osym: Symbol,
                         sym_ctrl: SymbolStats,
                         qrec: QRecBase,
                         **kwargs) -> Tuple[Symbol, QRecBase]:
        qtypes = kwargs.get('qtypes', {})
        # first see if this has already been quantized by nntool
        # note that the qtype will be stored against the name of the output symbol
        max_val, out_dtype, out_q = cls._get_scale_dtype_from_qtypes(osym, qtypes)
        if max_val is not None:
            qrec_out = Q15ScaleQRec(out_dtype, max_val, out_q)
            # scale clip and cast to output type
            return (ConvertFloatScaled(qsym, from_qrec=qrec, to_qrec=qrec_out), qrec_out)
        if not out_dtype:
            out_dtype = kwargs.get('out_dtype', np.float32)
        # Just cast
        return (Cast(qsym, dtype=out_dtype), FloatQRec(dtype=out_dtype, min_val=qrec.min_val, max_val=qrec.max_val))

    @classmethod
    def _get_scale_dtype_from_qtypes(cls, sym, qtypes):
        if not qtypes or sym.name not in qtypes:
            return None, None, None
        qtype = qtypes[sym.name]
        if qtype.dtype == np.int8:
            if len(qtype.scale) > 1:
                return None, None, None
            return qtype.scale[0] * math.pow(2, 7), np.int8, 7
        elif qtype.dtype == np.int16:
            if len(qtype.scale) > 1:
                return None, None, None
            return qtype.scale[0] * math.pow(2, 15), np.int16, 15
        else:
            return None, qtype.dtype, None
