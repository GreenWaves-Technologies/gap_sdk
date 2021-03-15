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
from utils.exp_17_15 import exp_fp_17_15
from utils.pow_sqrt import (arctan_17_15alt, logn_17_15, pow_17_15, sqrt_17_15,
                            square_17_15)

from ..basic import (Abs, Add, ATan, Cast, Div, Exp, GapAbs, GapMax, GapMin, Log, LShift, Max, Min,
                     Mul, Pow, Sqrt, Sub)
from ..function import Function
from ..quantization_base import qhandler
from ..symbol import (Constant, Rational, Symbol, SymbolStats, Variable,
                      environment, nargs, c_headers)
from .clip_norm import Norm
from .q15_scale_float import Q15ScaleFloat
from .q15_scale_q_rec import Q15ScaleQRec
from .q15_scaled_quantization import Q15ScaledQuantization
from .quantized_constant import QuantizedConstant
from .scale_quantized import ScaleQuantized


@qhandler("Q15Scale", Constant, Rational)
class BasicConstantQuant(Q15ScaledQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol, sym_ctrl:
                  SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:
        if isinstance(sym, QuantizedConstant):
            return (sym, Q15ScaleQRec(sym.dtype, 1, 0))
        if sym.is_zero:
            return (sym, Q15ScaleQRec(sym.dtype, 1, 15))
        if len(sym.value) == 1:
            return (QuantizedConstant(1), Q15ScaleQRec(np.int32, sym.value[0], 0))
        # iinfo = np.iinfo(qdtype)
        # if np.all(sym.value == np.floor(sym.value)) and np.all(np.abs(sym.value) < iinfo.max):
        #     return (QuantizedConstant(sym.value.astype(qdtype)), Q15ScaleQRec(qdtype, 1, 0))
        max_val = sym_ctrl.get_max(sym)
        # scale constants to Q15 by default
        qdtype = qrec.dtype if qrec and qrec.dtype else np.int16
        q = cls.get_maxq_from_dtype(qdtype)
        qrec = Q15ScaleQRec.inherit(qrec, np.int32, max_val, q)
        return (QuantizedConstant(qrec.quantize_and_clip(sym.value)), qrec)


@qhandler("Q15Scale", Variable)
class BasicVariableQuant(Q15ScaledQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        quantize_inputs = kwargs.get('quantize_inputs', None)
        prequantized_variables = kwargs.get('prequantized_variables', {})

        # if the symbol is in prequantized_variables it is probably
        # an output from a previous function so already has a quant record
        if sym.name in prequantized_variables:
            sym.qrec = prequantized_variables[sym.name]
            return (sym, prequantized_variables[sym.name])

        # figure out the quantization from the maximum value recorded
        max_val = sym_ctrl.get_max(sym)
        qdtype = qrec.dtype if qrec and qrec.dtype else np.int8
        q = cls.get_maxq_from_dtype(qdtype)
        qrec = Q15ScaleQRec.inherit(qrec, np.int8, max_val, q)
        # tag the variable with its quantization
        sym.qrec = qrec
        if quantize_inputs:
            return (Q15ScaleFloat(sym, to_qrec=qrec), qrec)
        return (sym, qrec)


class BasicFunctionQuant(Q15ScaledQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:
        raise NotImplementedError()

    @staticmethod
    def cast_symbols(in_syms, qrecs):
        return zip(*[(Cast(sym, dtype=np.int32), Q15ScaleQRec.override(qrec, dtype=np.int32))
                     if qrec.dtype != np.int32 else (sym, qrec)
                     for sym, qrec in zip(in_syms, qrecs)])


@qhandler("Q15Scale", Add, Sub, Min, Max)
class BasicEqualizeQ15Quant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:
        sym_cls = sym.__class__
        if sym_cls == Min:
            sym_cls = GapMin
        elif sym_cls == Max:
            sym_cls = GapMax

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])

        max_val = sym_ctrl.get_max(sym)
        out_qrec = Q15ScaleQRec(np.int32, max_val, 15)
        is_zero = list(in_sym.is_zero for in_sym in in_syms)
        in_syms, in_qrecs = cls.cast_symbols(in_syms, in_qrecs)
        # scale to the larger Q but not more than Q15
        calc_qrec = None
        scale_to = None

        if in_qrecs[0].scaledq != in_qrecs[1].scaledq and not any(is_zero):
            # if one argument is constant then scale it. It will be reduced to
            # something that fits
            if in_syms[0].is_constant and not in_syms[1].is_constant:
                scale_to = 1
            elif in_syms[1].is_constant and not in_syms[0].is_constant:
                scale_to = 0
            elif in_qrecs[0].scaledq < in_qrecs[1].scaledq:
                scale_to = 1
            elif in_qrecs[0].scaledq > in_qrecs[1].scaledq:
                scale_to = 0

        if scale_to == 0:
            in_syms = [
                in_syms[0],
                ScaleQuantized(in_syms[1], from_qrec=in_qrecs[1], to_qrec=in_qrecs[0])]
            calc_qrec = Q15ScaleQRec.override(in_qrecs[0], dtype=np.int32)
        elif scale_to == 1:
            in_syms = [
                ScaleQuantized(in_syms[0], from_qrec=in_qrecs[0], to_qrec=in_qrecs[1]),
                in_syms[1]]
            calc_qrec = Q15ScaleQRec.override(in_qrecs[1], dtype=np.int32)
        else:
            non_zero_index = next(iter([idx for idx, val in enumerate(is_zero) if not val]), None)
            if non_zero_index is None or in_qrecs[non_zero_index] == out_qrec:
                return sym_cls(*in_syms), out_qrec
            calc_qrec = Q15ScaleQRec.override(in_qrecs[non_zero_index], dtype=np.int32)

        # Try not to scale if we are still in bounds
        # This creates more error -> if np.abs(calc_qrec.quantize(max_val)) < max_short or 
        if calc_qrec == out_qrec:
            return (sym_cls(*in_syms), calc_qrec)

        return (ScaleQuantized(sym_cls(*in_syms),
                               from_qrec=calc_qrec, to_qrec=out_qrec), out_qrec)

@qhandler("Q15Scale", Mul)
class BasicMulQ15Quant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])
        sym_cls = sym.__class__
        if any(in_sym.is_zero for in_sym in in_syms):
            return (QuantizedConstant(0), Q15ScaleQRec(np.int32, 1, 15))
        in_syms, in_qrecs = cls.cast_symbols(in_syms, in_qrecs)
        prod_scale = in_qrecs[0].scale * in_qrecs[1].scale
        prod_q = in_qrecs[0].q + in_qrecs[1].q
        out_qrec = Q15ScaleQRec(np.int32, prod_scale, min(prod_q, 15))
        if prod_q > 15:
            qsym = Norm(sym_cls(*in_syms), QuantizedConstant(prod_q - 15))
        else:
            qsym = sym_cls(*in_syms)
        return (qsym, out_qrec)


@qhandler("Q15Scale", Div)
class BasicDivQ15Quant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])

        # LHS and RHS must be Q15 maximum
        # Then move both to the highest Q and shift lhs by that Q
        (lhs, rhs), (lhs_qrec, rhs_qrec) = cls.cast_symbols(in_syms, in_qrecs)
        lhs_adjust = 15 - lhs_qrec.q if lhs_qrec.q > 15 else 0
        rhs_adjust = 15 - rhs_qrec.q if rhs_qrec.q > 15 else 0

        if (lhs_qrec.q + lhs_adjust) < (rhs_qrec.q + rhs_adjust):
            lhs_adjust += (rhs_qrec.q + rhs_adjust) - (lhs_qrec.q + lhs_adjust)
        elif (lhs_qrec.q + lhs_adjust) > (rhs_qrec.q + rhs_adjust):
            rhs_adjust += (lhs_qrec.q + lhs_adjust) - (rhs_qrec.q + rhs_adjust)

        res_q = (lhs_qrec.q + lhs_adjust)
        lhs_adjust += res_q

        if lhs_adjust < 0:
            lhs = Norm(lhs, QuantizedConstant(-lhs_adjust))
        elif lhs_adjust > 0:
            lhs = LShift(lhs, QuantizedConstant(lhs_adjust))
        if rhs_adjust < 0:
            rhs = Norm(rhs, QuantizedConstant(-rhs_adjust))
        elif rhs_adjust > 0:
            rhs = LShift(rhs, QuantizedConstant(rhs_adjust))

        prod_scale = in_qrecs[0].scale / in_qrecs[1].scale
        calc_qrec = Q15ScaleQRec(np.int32, prod_scale, res_q)
        # No scaling necessary because the scale will be correct and the q must be 15 or less
        return (Div(lhs, rhs, is_floor=True), calc_qrec)


@nargs(1)
@c_headers('"math_funcs.h"')
@environment({
    'sqrt_17_15': sqrt_17_15,
})
class Sqrt1715(Function):

    def _impl(self, *args, **kwargs):
        return sqrt_17_15(np.array(args[0]))

    def _py_expr(self, *args, **kwargs):
        return "sqrt_17_15(np.array(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "sqrt_17_15(%s)" % (args[0],)


@qhandler("Q15Scale", Sqrt)
class BasicQ17Q15SqrtQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])
        in_qrec = in_qrecs[0]
        new_scale = np.sqrt(in_qrec.scale)
        in_syms, in_qrecs = cls.cast_symbols(in_syms, in_qrecs)
        in_sym = in_syms[0]
        if in_qrec.q < 15:
            in_sym = LShift(in_sym, QuantizedConstant(15 - in_qrec.q))
        elif in_qrec.q > 15:
            in_sym = Norm(in_sym, QuantizedConstant(in_qrec.q - 15))

        out_qrec = Q15ScaleQRec(np.int32, new_scale, 15)
        return (Sqrt1715(in_sym), out_qrec)


@nargs(1)
@environment({
    'logn_17_15': logn_17_15,
})
@c_headers('"math_funcs.h"')
class Log1715(Function):

    def _impl(self, *args, **kwargs):
        return logn_17_15(np.array(args[0]))

    def _py_expr(self, *args, **kwargs):
        return "logn_17_15(np.array(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "logn_17_15(%s)" % (args[0],)


@qhandler("Q15Scale", Log)
class BasicQ17Q15LogQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])
        in_qrec = in_qrecs[0]
        log_off = np.log(in_qrec.scale)
        in_syms, in_qrecs = cls.cast_symbols(in_syms, in_qrecs)
        in_sym = in_syms[0]
        if in_qrec.q < 15:
            in_sym = LShift(in_sym, QuantizedConstant(15 - in_qrec.q))
        elif in_qrec.q > 15:
            in_sym = Norm(in_sym, QuantizedConstant(in_qrec.q - 15))

        # log(Qx * scale) = log(Qx) + log(scale)
        max_val = sym_ctrl.get_max(sym)
        out_qrec = Q15ScaleQRec(np.int32, max_val, 15)
        calc_qrec = Q15ScaleQRec(np.int32, 1, 15)
        qlog_off = calc_qrec.quantize_and_clip(log_off)
        # logn_17_15(1) is -340695 which uses 19 bits. The offset is added before scaling so calculate
        # the number of bits at minimum left after the addition and fit the scaling in that
        max_bits = math.ceil(math.log2(math.fabs(-340695 + qlog_off))) + 2
        return (
            ScaleQuantized(Add(Log1715(in_sym), QuantizedConstant(qlog_off)), from_qrec=calc_qrec, to_qrec=out_qrec, num_bits=31-max_bits),
            out_qrec)


@nargs(2)
@environment({
    'pow_17_15': pow_17_15,
})
@c_headers('"math_funcs.h"')
class Pow1715(Function):

    def _impl(self, *args, **kwargs):
        return pow_17_15(np.array(args[0]), np.array(args[1]))

    def _py_expr(self, *args, **kwargs):
        return "pow_17_15(np.array(%s), np.array(%s))" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return "pow_17_15(%s, %s)" % (args[0], args[1])


@nargs(1)
@environment({
    'square_17_15': square_17_15,
})
@c_headers('"math_funcs.h"')
class Square1715(Function):

    def _impl(self, *args, **kwargs):
        return square_17_15(np.array(args[0]))

    def _py_expr(self, *args, **kwargs):
        return "square_17_15(np.array(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "square_17_15(%s)" % (args[0],)


@qhandler("Q15Scale", Pow)
class BasicQ17Q15PowQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])
        rhs = in_syms[1].resolve()
        if not isinstance(rhs, Constant):
            raise NotImplementedError(
                "power is currently only supported with fractional constants, 2, 1, & 0")

        val = np.round(rhs.value.astype(np.float) * in_qrecs[1].scaledq, decimals=4)
        lhs, lhs_qrec = cls.cast_symbols(in_syms[:1:], in_qrecs[:1:])
        lhs, lhs_qrec = lhs[0], lhs_qrec[0]
        if lhs_qrec.q < 15:
            lhs = LShift(lhs, QuantizedConstant(15 - lhs_qrec.q, dtype=np.int8))
        elif lhs_qrec.q > 15:
            lhs = Norm(lhs, QuantizedConstant(lhs_qrec.q - 15, dtype=np.int8))

        if val == 2:
            out_qrec = Q15ScaleQRec(np.int32, np.power(lhs_qrec.scale, 2), 15)
            return (Square1715(lhs), out_qrec)
        if val == -2:
            out_qrec = Q15ScaleQRec(np.int32, 1/np.power(lhs_qrec.scale, 2), 15)
            return (Div(QuantizedConstant(1 << 30), Square1715(lhs)), out_qrec)
        if val == 1:
            out_qrec = Q15ScaleQRec(np.int32, lhs_qrec.scale, 15)
            return (lhs, out_qrec)
        if val == 0:
            out_qrec = Q15ScaleQRec(np.int32, 1, 0)
            return (QuantizedConstant(1), out_qrec)
        if val > 0 and val < 1:
            out_qrec = Q15ScaleQRec(np.uint32, np.power(lhs_qrec.scale, val), 15)
            qval = int(math.floor(val * math.pow(2, 15) + 0.5))
            return (Pow1715(lhs, QuantizedConstant(qval)), out_qrec)
        raise NotImplementedError(
            "power is currently only supported with fractional constants, 2, 1, & 0")


@nargs(1)
@environment({
    'exp_fp_17_15': exp_fp_17_15,
})
@c_headers('"math_funcs.h"')
class Exp1715(Function):

    def _impl(self, *args, **kwargs):
        return exp_fp_17_15(np.array(args[0]))

    def _py_expr(self, *args, **kwargs):
        return "exp_fp_17_15(np.array(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "exp_17_15(%s)" % (args[0],)


@qhandler("Q15Scale", Exp)
class BasicQ17Q15ExpQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])

        # e^xy = e^x^y so have to be in scale 1
        calc_qrec = Q15ScaleQRec(np.int32, 1, 15)
        in_syms, in_qrecs = cls.cast_symbols(in_syms, in_qrecs)
        arg = ScaleQuantized(in_syms[0], from_qrec=in_qrecs[0], to_qrec=calc_qrec)
        max_val = sym_ctrl.get_max(sym)
        out_qrec = Q15ScaleQRec(np.int32, max_val, 15)

        return (ScaleQuantized(Cast(Exp1715(arg), dtype=np.int32),
                               from_qrec=calc_qrec,
                               to_qrec=out_qrec),
                out_qrec)


@nargs(1)
@environment({
    'arctan_17_15alt': arctan_17_15alt,
})
@c_headers('"math_funcs.h"')
class Arctan1715(Function):

    def _impl(self, *args, **kwargs):
        return arctan_17_15alt(np.array(args[0]))

    def _py_expr(self, *args, **kwargs):
        return "arctan_17_15alt(np.array(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "arctan_17_15(%s)" % (args[0],)


@qhandler("Q15Scale", ATan)
class BasicQ17Q15ArctanQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])

        calc_qrec = Q15ScaleQRec(np.int32, 1, 15)
        in_syms, in_qrecs = cls.cast_symbols(in_syms, in_qrecs)
        lhs = ScaleQuantized(in_syms[0], from_qrec=in_qrecs[0], to_qrec=calc_qrec)
        return (Arctan1715(lhs), calc_qrec)


@qhandler("Q15Scale", Abs)
class BasicQ17Q15NoChangeQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: Q15ScaleQRec = None,
                  **kwargs) -> Tuple[Symbol, Q15ScaleQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])
        sym_cls = sym.__class__
        if isinstance(sym, Abs):
            sym_cls = GapAbs

        return (sym_cls(*in_syms), in_qrecs[0])
