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
from bfloat16 import bfloat16
from expressions.symbolic.float_quantization.float_qrec import FloatQRec

from ..basic import Cast, ConvertQuantization
from ..quantization_base import (QRecBase, QuantizationHandlerBase,
                                 handles_scheme)
from ..symbol import Symbol, SymbolStats
from .clip_norm import Clip
from .q15_scale_q_rec import Q15ScaleQRec
from .scale_quantized import ScaleQuantized


@handles_scheme('Q15Scale')
class Q15ScaledQuantization(QuantizationHandlerBase):
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

    # @classmethod
    # def _quantize_output(cls,
    #                      sym: Symbol,
    #                      qsym: Symbol,
    #                      osym: Symbol,
    #                      sym_ctrl: SymbolStats,
    #                      qrec: QRecBase,
    #                      **kwargs) -> Tuple[Symbol, QRecBase]:
    #     from_qrec = qrec
    #     qtypes = kwargs.get('qtypes', {})
    #     # first see if this has already been quantized by nntool
    #     # note that the qtype will be stored against the name of the output symbol
    #     res = cls._get_scale_dtype_from_qtypes(
    #         osym, qtypes)
    #     if res is None:
    #         max_val = math.fabs(sym_ctrl.get_max(sym))
    #         min_val = -max_val
    #         out_dtype = np.int8
    #         out_q = 7
    #         zero_point = 0
    #     else:
    #         max_val, min_val, out_dtype, out_q, zero_point = res

    #     qrec_scale = Q15ScaleQRec(np.int32, max_val, out_q, min_val=min_val, max_val=max_val, zero_point=zero_point)
    #     qrec_out = Q15ScaleQRec(out_dtype, max_val, out_q, min_val=min_val, max_val=max_val, zero_point=zero_point)
    #     # scale clip and cast to output type
    #     return (
    #         Cast(
    #             Clip(
    #                 ScaleQuantized(qsym,
    #                                from_qrec=from_qrec,
    #                                to_qrec=qrec_scale),
    #                 clip_dtype=out_dtype,
    #                 dtype=qrec_scale.dtype),
    #             dtype=qrec.dtype), qrec_out)

    # @classmethod
    # def _get_scale_dtype_from_qtypes(cls, sym, qtypes):
    #     if not qtypes or sym.name not in qtypes:
    #         return None
    #     qtype = qtypes[sym.name]
    #     if qtype.dtype in [np.int8, np.uint8, np.int16, np.uint16]:
    #         if len(qtype.scale) > 1:
    #             return None
    #         max_val, min_val, bitlen = Q15ScaleQRec.dtype_zp_to_min_max(qtype.dtype, qtype.scale[0], qtype.zero_point)
    #         return max_val, min_val, qtype.dtype, bitlen, qtype.zero_point
    #     else:
    #         return None

    @classmethod
    def _quantize_output(cls,
                         sym: Symbol,
                         qsym: Symbol,
                         osym: Symbol,
                         sym_ctrl: SymbolStats,
                         qrec: QRecBase,
                         **kwargs) -> Tuple[Symbol, QRecBase]:
        from_qrec = qrec
        qtypes = kwargs.get('qtypes', {})
        # first see if this has already been quantized by nntool
        # note that the qtype will be stored against the name of the output symbol

        if qtypes and osym.name in qtypes:
            qtype = qtypes[osym.name]
            if qtype.dtype in [np.int8, np.uint8, np.int16, np.uint16]:
                max_val, min_val, out_q = Q15ScaleQRec.dtype_zp_to_min_max(
                    qtype.dtype, qtype.scale[0], qtype.zero_point)
                out_dtype = qtype.dtype
                zero_point = qtype.zero_point
            elif qtype.dtype in [np.float16, bfloat16, np.float32]:
                min_val = qtype.min_val
                max_val = qtype.max_val
                out_dtype = qtype.dtype
            else:
                raise ValueError(f"don't know how to output {qtype.dtype}")
        else:
            out_dtype = kwargs.get('out_dtype', np.int8)
            assert out_dtype in [np.int8, np.int16]
            max_val = math.fabs(sym_ctrl.get_max(sym))
            min_val = -max_val
            out_dtype = out_dtype
            out_q = 7 if out_dtype == np.int8 else 15
            zero_point = 0

        if out_dtype in [np.float16, bfloat16, np.float32]:
            qrec_out = FloatQRec(
                dtype=out_dtype, max_val=max_val, min_val=min_val)
            return (
                ConvertQuantization(
                    qsym,
                    from_qrec=from_qrec,
                    to_qrec=qrec_out,
                    comment=f'convert quantization - {from_qrec} -> {qrec_out}'
                ), qrec_out)

        qrec_scale = Q15ScaleQRec(
            np.int32, max_val, out_q, min_val=min_val, max_val=max_val, zero_point=zero_point)
        qrec_out = Q15ScaleQRec(
            out_dtype, max_val, out_q, min_val=min_val, max_val=max_val, zero_point=zero_point)
        return (
            Cast(
                Clip(
                    ScaleQuantized(
                        qsym,
                        from_qrec=from_qrec,
                        to_qrec=qrec_scale
                    ),
                    clip_dtype=out_dtype,
                    dtype=qrec_scale.dtype
                ),
                dtype=qrec_out.dtype,
                comment=f'scale clip and cast - {from_qrec} -> {qrec_out}'
            ),
            qrec_out)
