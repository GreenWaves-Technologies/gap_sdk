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
from nntool.expressions.symbolic.float_quantization.float_qrec import FloatQRec

from nntool.expressions.symbolic.q15_quantization.scale_quantized import ScaleQuantized

from ..basic import Cast, Clip, ConvertQuantization
from ..q15_quantization.q15_scale_q_rec import Q15ScaleQRec
from ..quantization_base import (QRecBase, QuantizationHandlerBase,
                                 handles_scheme)
from ..symbol import Symbol, SymbolStats


@handles_scheme('Float')
#pylint: disable=abstract-method
class FloatQ15QuantizationBase(QuantizationHandlerBase):

    @classmethod
    def _fixed_output(cls, sym, in_qrec, scale_qrec, out_qrec):
        return (
            Cast(
                Clip(
                    ScaleQuantized(
                        sym,
                        from_qrec=in_qrec,
                        to_qrec=scale_qrec
                    ),
                    clip_dtype=out_qrec.dtype,
                    dtype=scale_qrec.dtype
                ),
                dtype=out_qrec.dtype,
                comment=f'scale clip and cast - {in_qrec} -> {out_qrec}'
            ),
            out_qrec)

    @classmethod
    def _quantize_output(cls,
                         sym: Symbol,
                         qsym: Symbol,
                         osym: Symbol,
                         sym_ctrl: SymbolStats,
                         qrec: QRecBase,
                         **kwargs) -> Tuple[Symbol, QRecBase]:
        # This handles output quantization of both fixed point and float
        # quantized expressions and float and fixed point outputs

        qtypes = kwargs.get('qtypes', {})
        in_dtype_is_float = qrec.dtype in [np.float32, np.float16, bfloat16]

        # first see if this has already been quantized by nntool
        # note that the qtype will be stored against the name of the output symbol
        # If there is no qtype this is most certainly a test
        if not qtypes or osym.name not in qtypes:
            out_dtype = kwargs.get(
                'out_dtype', np.float32 if in_dtype_is_float else np.int8)
            out_dtype_is_float = qrec.dtype in [
                np.float32, np.float16, bfloat16]
            if in_dtype_is_float:
                if out_dtype_is_float:
                    out_qrec = FloatQRec(out_dtype)
                    return (Cast(qsym, dtype=out_dtype), out_qrec)
                raise NotImplementedError(
                    'float to fixed type change on output without qtypes - testing only')

            out_dtype = kwargs.get('out_dtype', np.int8)
            assert out_dtype in [np.int8, np.int16]
            max_val = math.fabs(sym_ctrl.get_max(sym))
            min_val = -max_val
            out_q = 7 if out_dtype == np.int8 else 15
            zero_point = 0

            scale_qrec = Q15ScaleQRec(
                np.int32, max_val, out_q, min_val=min_val, max_val=max_val, zero_point=zero_point)
            out_qrec = Q15ScaleQRec(
                out_dtype, max_val, out_q, min_val=min_val, max_val=max_val, zero_point=zero_point)
            return cls._fixed_output(qsym, qrec, scale_qrec, out_qrec)

        # OK. We have a QType for this output
        # Now tack on the correct stuff to the quantized symbol depending on type conversion
        # float -> float, float -> fix, fix -> float, fix -> fix
        qtype = qtypes[osym.name]
        if qtype.dtype in [np.int8, np.uint8, np.int16, np.uint16]:
            #
            # FIX OUT STUFF
            if in_dtype_is_float:
                # float in fix out
                if len(qtype.scale) > 1:
                    out_dtype = kwargs.get('out_dtype', np.float32)
                    out_qrec = FloatQRec(out_dtype)
                    return (Cast(qsym, dtype=out_dtype), out_qrec)
                out_qrec = Q15ScaleQRec.from_qtype(qtype)
                return (
                    ConvertQuantization(
                        qsym,
                        from_qrec=qrec,
                        to_qrec=out_qrec,
                        comment=f'convert quantization - {qrec} -> {out_qrec}'
                    ), out_qrec)

            # fix in fix out
            out_qrec = Q15ScaleQRec.from_qtype(qtype)
            scale_qrec = Q15ScaleQRec.override(out_qrec, dtype=np.int32)
            return cls._fixed_output(qsym, qrec, scale_qrec, out_qrec)
        #
        # FLOAT OUT STUFF
        if in_dtype_is_float:
            # float in float out
            out_qrec = FloatQRec(
                dtype=qtype.dtype, max_val=qtype.max_val, min_val=qtype.min_val)
            return (Cast(qsym, dtype=qtype.dtype), out_qrec)
        # fix in float out
        min_val = qtype.min_val
        max_val = qtype.max_val
        out_dtype = qtype.dtype
        qrec_out = FloatQRec(
            dtype=out_dtype, max_val=max_val, min_val=min_val)
        return (
            ConvertQuantization(
                qsym,
                from_qrec=qrec,
                to_qrec=qrec_out,
                comment=f'convert quantization - {qrec} -> {qrec_out}'
            ), qrec_out)
