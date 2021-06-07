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

from ..basic import (ATan, Abs, Add, Cast, Cos, Div, Exp, HSigmoid, HTanh, Log, Max, Min, Mul, Pow, Sigmoid,
                     Sin, Sqrt, Sub, TanH)
from ..function import Function
from ..quantization_base import qhandler
from ..symbol import (Constant, Rational, Symbol, SymbolStats, Variable,
                      c_headers, environment, nargs)
from .float_qrec import FloatQRec
from .float_quantization import FloatQuantization


@qhandler("Float", Constant, Rational)
class BasicConstantQuant(FloatQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:

        return sym, FloatQRec(dtype=np.float32, min_val=sym.value[0], max_val=sym.value[0])


@qhandler("Float", Variable)
class BasicVariableQuant(FloatQuantization):

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: FloatQRec = None,
                  **kwargs) -> Tuple[Symbol, FloatQRec]:

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
                return (sym, qrec)

        out_dtype = kwargs.get('out_dtype', np.float32)
        return FloatQRec(dtype=out_dtype, max_val=max_val, min_val=-max_val)

    @classmethod
    def qrec_from_qtype(cls, qtype, max_val):
        if qtype.dtype == np.int8 or qtype.dtype == np.int16:
            if qtype.dtype == np.int8:
                if len(qtype.scale) > 1:
                    qtype.scale = np.max(qtype.scale)
                q = 7
                dtype = np.int8
            elif qtype.dtype == np.int16:
                if len(qtype.scale) > 1:
                    qtype.scale = np.max(qtype.scale)
                q = 15
                dtype = np.int16
            return Q15ScaleQRec(dtype, max_val, q, max_val=max_val, min_val=-max_val)
        elif qtype.dtype == np.float32 or qtype.dtype == bfloat16:
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
          Add, Sub, Min, Max, Mul, Div, Sqrt, Abs, Log, Exp, Pow, Sigmoid, HSigmoid, ATan, TanH, HTanh)
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


@qhandler("Float", Cos, Sin)
class FastFloatSinCosQuant(BasicFunctionQuant):

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
        else:
            qsym = FastFloatSin
        return (qsym(*in_syms), out_qrec)
