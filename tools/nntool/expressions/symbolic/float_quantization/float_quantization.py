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

from ..basic import Cast, ConvertQuantization
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
        if not qtypes or osym.name not in qtypes:
            out_dtype  = kwargs.get('out_dtype', np.float32)
            qrec_out = FloatQRec(out_dtype)
            return (Cast(qsym, dtype=out_dtype), qrec_out)

        qtype = qtypes[osym.name]
        if qtype.dtype in [np.int8, np.uint8, np.int16, np.uint16]:
            if len(qtype.scale) > 1:
                out_dtype  = kwargs.get('out_dtype', np.float32)
                qrec_out = FloatQRec(out_dtype)
                return (Cast(qsym, dtype=out_dtype), qrec_out)
            max_val, min_val, bitlen = Q15ScaleQRec.dtype_zp_to_min_max(qtype.dtype, qtype.scale[0], qtype.zero_point)
            qrec_out = Q15ScaleQRec(qtype.dtype, max_val, bitlen, min_val=min_val, max_val=max_val, zero_point=qtype.zero_point)
            return (ConvertQuantization(qsym, from_qrec=qrec, to_qrec=qrec_out), qrec_out)
        qrec_out = FloatQRec(dtype=qtype.dtype, max_val=qtype.max_val, min_val=qtype.min_val)
        return (Cast(qsym, dtype=qtype.dtype),qrec_out)
