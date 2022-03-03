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
from bfloat16 import bfloat16
from expressions.symbolic.float_quantization.float_qrec import FloatQRec
from expressions.symbolic.q15_quantization.q15_scale_q_rec import Q15ScaleQRec
from utils.fast_float import (np_fastexp, np_fastlog, np_fastlog2, np_fastpow,
                              np_fastpow2, np_fastrsqrt, np_fastsigmoid,
                              np_fasttanh)

from ..basic import (Abs, Add, ATan, Cast, ConvertQuantization, Cos, Div, Exp, HSigmoid, HTanh, Log,
                     Max, Min, Mul, Pow, RSqrt, Sigmoid, Sin, Sqrt, Sub, TanH)
from ..function import Function
from ..quantization_base import qhandler
from ..symbol import (Constant, Rational, Symbol, SymbolStats, Variable,
                      c_headers, environment, nargs)
from .float_qrec import FloatQRec
from .float_quantization import FloatQuantization

# from utils.sigmoid_tanh_lut import sigmoid_lut_float, tanh_lut_float


@qhandler("Float", Constant, Rational)
class BasicConstantQuant(FloatQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:

        return sym, FloatQRec(dtype=bfloat16, min_val=sym.value[0], max_val=sym.value[0])


@qhandler("Float", Variable)
class BasicVariableQuant(FloatQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:
        # TODO: Needs merging with Q15 version
        prequantized_variables = kwargs.get('prequantized_variables', {})
        qtypes = kwargs.get('qtypes', {})

        # if the symbol is in prequantized_variables it is probably
        # an output from a previous function so already has a quant record
        if sym.name in prequantized_variables:
            sym.qrec = prequantized_variables[sym.name]
            return (sym, prequantized_variables[sym.name])

        max_val = sym_ctrl.get_max(sym)

        # see if an nntool quantizer qtype is available
        if not qrec and qtypes and sym.name in qtypes:
            qrec = cls.qrec_from_qtype(qtypes[sym.name], max_val)
            if qrec:
                sym.qrec = qrec
                if isinstance(qrec, FloatQRec):
                    return (sym, qrec)
                out_dtype = kwargs.get('out_dtype', np.float32)
                out_qrec = FloatQRec(dtype=out_dtype, max_val=max_val, min_val=-max_val)
                return (
                    ConvertQuantization(sym, from_qrec=qrec, to_qrec=out_qrec, tag=sym.name),
                    out_qrec)

        out_dtype = kwargs.get('out_dtype', np.float32)
        return sym, FloatQRec(dtype=out_dtype, max_val=max_val, min_val=-max_val)

    @classmethod
    def qrec_from_qtype(cls, qtype, max_val):
        if qtype.dtype in [np.int8, np.uint8, np.int16, np.uint16]:
            if len(qtype.scale) > 1:
                return None
            max_val, min_val, bitlen = Q15ScaleQRec.dtype_zp_to_min_max(
                qtype.dtype, qtype.scale[0], qtype.zero_point[0])
            return Q15ScaleQRec(qtype.dtype, max_val, bitlen,
                                max_val=max_val, min_val=min_val,
                                zero_point=qtype.zero_point[0])
        elif qtype.dtype in [np.float32, np.float16, bfloat16]:
            return FloatQRec(dtype=qtype.dtype, max_val=max_val, min_val=-max_val)
        else:
            return None


class BasicFunctionQuant(FloatQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:
        raise NotImplementedError()

    @staticmethod
    def cast_symbols(in_syms, qrecs, dtype=np.float32):
        return zip(*[(Cast(sym, dtype=dtype), FloatQRec.override(qrec, dtype=dtype))
                     if qrec.dtype != dtype else (sym, qrec)
                     for sym, qrec in zip(in_syms, qrecs)])


@qhandler("Float",
          Add, Sub, Min, Max, Mul, Div, Sqrt, HSigmoid, ATan, HTanh)
class BasicFloatFuncQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])
        sym_cls = sym.__class__
        in_syms, in_qrecs = cls.cast_symbols(in_syms, in_qrecs)

        max_val = sym_ctrl.get_max(sym)
        min_val = None if max_val is None else -max_val
        out_qrec = FloatQRec(dtype=np.float32,
                             min_val=min_val, max_val=max_val)

        return (Cast(sym_cls(*in_syms), dtype=np.float32), out_qrec)


@nargs(1)
@c_headers('CNN_Defines_fp16.h')
class AbsF(Function):

    def _impl(self, *args, **kwargs):
        return np.abs(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.abs(%s)" % args[0]

    def _c_expr(self, *args, **kwargs):
        return "AbsF(%s)" % (args[0])

# TODO - Need numpy equivalents of sin and cos
# TODO - All of these should return correct function based on output type (i.e. bfloat16/ieee16 version)


@nargs(1)
@environment({
    'npcos': np.cos,
})
@c_headers('"float_math_funcs.h"')
class FastFloatCos(Function):

    def _impl(self, *args, **kwargs):
        return np.cos(np.array(args[0]))

    def _py_expr(self, *args, **kwargs):
        return "npcos(np.array(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return f"ffast_cos({args[0]})"


@nargs(1)
@environment({
    'npsin': np.sin,
})
@c_headers('"float_math_funcs.h"')
class FastFloatSin(Function):

    def _impl(self, *args, **kwargs):
        return np.sin(np.array(args[0]))

    def _py_expr(self, *args, **kwargs):
        return "npsin(np.array(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return f"ffast_sin({args[0]})"


@nargs(1)
@environment({
    'np_fastsigmoid': np_fastsigmoid,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatSigmoid(Function):

    def _impl(self, *args, **kwargs):
        return np_fastsigmoid(np.array(args[0]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fastsigmoid(np.array({args[0]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fastsigmoid({args[0]})"


@nargs(1)
@environment({
    'np_fasttanh': np_fasttanh,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatTanH(Function):

    def _impl(self, *args, **kwargs):
        return np_fasttanh(np.array(args[0]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fasttanh(np.array({args[0]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fasttanh({args[0]})"


@nargs(2)
@environment({
    'np_fastpow': np_fastpow,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatPow(Function):

    def _impl(self, *args, **kwargs):
        return np_fastpow(np.array(args[0]), np.array(args[1]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fastpow(np.array({args[0]}), np.array({args[1]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fastpow({args[0]}, {args[1]})"


@nargs(1)
@environment({
    'np_fastpow2': np_fastpow2,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatPow2(Function):

    def _impl(self, *args, **kwargs):
        return np_fastpow2(np.array(args[0]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fastpow2(np.array({args[0]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fastpow2({args[0]})"


@nargs(1)
@environment({
    'np_fastlog': np_fastlog,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatLog(Function):

    def _impl(self, *args, **kwargs):
        return np_fastlog(np.array(args[0]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fastlog(np.array({args[0]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fastlog({args[0]})"


@nargs(1)
@environment({
    'np_fastlog2': np_fastlog2,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatLog2(Function):

    def _impl(self, *args, **kwargs):
        return np_fastlog2(np.array(args[0]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fastlog2(np.array({args[0]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fastlog2({args[0]})"


@nargs(1)
@environment({
    'np_fastexp': np_fastexp,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatExp(Function):

    def _impl(self, *args, **kwargs):
        return np_fastexp(np.array(args[0]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fastexp(np.array({args[0]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fastexp({args[0]})"


@nargs(1)
@environment({
    'np_fastrsqrt': np_fastrsqrt,
})
@c_headers('"FastFloatApprox.h"')
class FastFloatRSqrt(Function):

    def _impl(self, *args, **kwargs):
        return np_fastrsqrt(np.array(args[0]), dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np_fastrsqrt(np.array({args[0]}))"

    def _c_expr(self, *args, **kwargs):
        return f"fastrsqrt({args[0]})"


@qhandler("Float", Cos, Sin, Sigmoid, TanH, Abs, Pow, Exp, Log, RSqrt)
class FastFloatExprFunQuant(BasicFunctionQuant):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:

        in_syms, in_qrecs = zip(*[cls.quantize(inner_sym, sym_ctrl, **kwargs)
                                  for inner_sym in sym.contents])

        in_syms, in_qrecs = cls.cast_symbols(
            in_syms, in_qrecs, dtype=np.float32)

        max_val = sym_ctrl.get_max(sym)
        min_val = None if max_val is None else -max_val
        out_qrec = FloatQRec(dtype=np.float32,
                             min_val=min_val, max_val=max_val)
        if isinstance(sym, Cos):
            qsym = FastFloatCos
        elif isinstance(sym, RSqrt):
            qsym = FastFloatRSqrt
        elif isinstance(sym, Sin):
            qsym = FastFloatSin
        elif isinstance(sym, Sigmoid):
            qsym = FastFloatSigmoid
        elif isinstance(sym, TanH):
            qsym = FastFloatTanH
        elif isinstance(sym, Pow):
            base_val = in_syms[0].resolve()
            if isinstance(base_val, Constant) and base_val.value == 2:
                in_syms = [in_syms[1]]
                qsym = FastFloatPow2
            else:
                qsym = FastFloatPow
        elif isinstance(sym, Log):
            qsym = FastFloatLog
        elif isinstance(sym, Exp):
            qsym = FastFloatExp
        elif isinstance(sym, Abs):
            if sym.dtype == np.float32:
                qsym = Abs
            else:
                qsym = AbsF
        return (qsym(*in_syms), out_qrec)
