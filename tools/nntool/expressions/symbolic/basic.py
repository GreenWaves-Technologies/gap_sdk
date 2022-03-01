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

import logging

import numpy as np
from bfloat16 import bfloat16
from quantization.qtype import DTYPE_GAP_CTYPE
from scipy.special import expit

from .function import Function
from .symbol import (Constant, Rational, c_headers, copy_props, environment,
                     handles, handlesr, nargs)

LOG = logging.getLogger('nntool.'+__name__)


@nargs(2)
@handles('__add__')
@handlesr('__radd__')
class Add(Function):

    def _impl(self, *args, **kwargs):
        return np.add(args[0], args[1], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.add(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return "(%s+%s)" % (args[0], args[1])


@nargs(2)
@handles('__mul__')
@handlesr('__rmul__')
class Mul(Function):

    def _impl(self, *args, **kwargs):
        return np.multiply(args[0], args[1], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.multiply(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return "(%s*%s)" % (args[0], args[1])

    # quantization strategy
    # if in Q <= 15 - mul without scaling
    # if in Q > 15 - reduce all to Q15 and mul


@nargs(2)
@handles('__sub__')
@handlesr('__rsub__')
class Sub(Function):

    def _impl(self, *args, **kwargs):
        return np.subtract(args[0], args[1], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.subtract(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return "(%s-%s)" % (args[0], args[1])


@nargs(2)
@handles('__div__')
@handles('__floordiv__', is_floor=True)
@handlesr('__rdiv__')
@handlesr('__rfloordiv__', is_floor=True)
@copy_props('is_floor')
class Div(Function):
    def __init__(self, *args, is_floor=False, **kwargs) -> None:
        super(Div, self).__init__(*args, **kwargs)
        self._is_floor = is_floor

    @property
    def is_floor(self):
        return self._is_floor

    def _impl(self, *args, **kwargs):
        if self.is_floor:
            return np.floor_divide(args[0], args[1], dtype=self.dtype)
        return np.true_divide(args[0], args[1], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        if self.is_floor:
            return "np.floor_divide(%s, %s)" % (args[0], args[1])
        return "np.true_divide(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return "(%s/%s)" % (args[0], args[1])


@nargs(2)
@handles('__lshift__')
@handlesr('__rlshift__')
class LShift(Function):

    def _impl(self, *args, **kwargs):
        return np.left_shift(args[0], args[1], dtype=np.int32)

    def _py_expr(self, *args, **kwargs):
        return "np.left_shift(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return "(%s<<%s)" % (args[0], args[1])


@nargs(2)
@handles('__rshift__')
@handlesr('__rrshift__')
class RShift(Function):

    def _impl(self, *args, **kwargs):
        return np.right_shift(args[0], args[1], dtype=np.int32)

    def _py_expr(self, *args, **kwargs):
        return "np.right_shift(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return "(%s>>%s)" % (args[0], args[1])


@nargs(1)
@handles('__neg__')
class Neg(Function):

    def _impl(self, *args, **kwargs):
        return np.negative(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.negative(%s)" % args[0]

    def _py_expr(self, *args, **kwargs):
        return "(-%s)" % args[0]


@nargs(1)
@handles('__pos__')
class Pos(Function):

    def _impl(self, *args, **kwargs):
        return args[0]

    def _py_expr(self, *args, **kwargs):
        return "%s" % args[0]

    def _eval(self, *args, **kwargs):
        return args[0]


@nargs(1)
@c_headers('"FloatDefines.h"')
class Abs(Function):

    def _impl(self, *args, **kwargs):
        return np.abs(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.abs(%s)" % args[0]

    def _c_expr(self, *args, **kwargs):
        return "Absf32(%s)" % (args[0])


@c_headers('"Gap.h"')
class GapAbs(Abs):
    def _c_expr(self, *args, **kwargs):
        return "gap_abs(%s)" % (args[0])


@nargs(1)
class Round(Function):

    def _impl(self, *args, **kwargs):
        return np.floor(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.round(%s)" % args[0]

    def _c_expr(self, *args, **kwargs):
        return "round(%s)" % args[0]


@nargs(1)
class Floor(Function):

    def _impl(self, *args, **kwargs):
        return np.floor(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.floor(%s)" % args[0]

    def _c_expr(self, *args, **kwargs):
        return "floor(%s)" % args[0]


@nargs(1)
@c_headers('<math.h>')
class Ceil(Function):

    def _impl(self, *args, **kwargs):
        return np.ceil(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.ceil(%s)" % args[0]

    def _c_expr(self, *args, **kwargs):
        return "ceil(%s)" % args[0]


@nargs(2)
@c_headers('"FloatDefines.h"')
class Max(Function):

    def _impl(self, *args, **kwargs):
        return np.maximum(args[0], args[1], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.maximum(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return f"Maxf32(({args[0]}),({args[1]}))"


@c_headers('"Gap.h"')
class GapMax(Max):
    def _c_expr(self, *args, **kwargs):
        return "gap_max(%s, %s)" % (args[0], args[1])


@nargs(2)
@c_headers('"FloatDefines.h"')
class Min(Function):

    def _impl(self, *args, **kwargs):
        return np.minimum(args[0], args[1], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.minimum(%s, %s)" % (args[0], args[1])

    def _c_expr(self, *args, **kwargs):
        return f"Minf32(({args[0]}),({args[1]}))"


@c_headers('"Gap.h"')
class GapMin(Min):
    def _c_expr(self, *args, **kwargs):
        return "gap_min(%s, %s)" % (args[0], args[1])


@nargs(1)
@c_headers('<math.h>')
class Sqrt(Function):

    def _impl(self, *args, **kwargs):
        return np.sqrt(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.sqrt(%s)" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "sqrtf(%s)" % (args[0],)


@nargs(1)
@c_headers('<math.h>')
class RSqrt(Function):

    def _impl(self, *args, **kwargs):
        return 1 / np.sqrt(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "(1/np.sqrt(%s))" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "1.0f/sqrtf(%s)" % (args[0],)


@nargs(1)
@c_headers('<math.h>')
class Log(Function):

    def _impl(self, *args, **kwargs):
        return np.log(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.log(%s)" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "logf(%s)" % (args[0],)


@nargs(1)
@c_headers('<math.h>')
class Sin(Function):

    def _impl(self, *args, **kwargs):
        return np.sin(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.sin(%s)" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "sin(%s)" % (args[0],)


@nargs(1)
@c_headers('<math.h>')
class Cos(Function):

    def _impl(self, *args, **kwargs):
        return np.cos(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.cos(%s)" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "cos(%s)" % (args[0],)


@nargs(1)
@c_headers('<math.h>')
class ATan(Function):

    def _impl(self, *args, **kwargs):
        return np.arctan(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.arctan(%s)" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "atan(%s)" % (args[0],)


@nargs(1)
@c_headers('<math.h>')
class Square(Function):

    def _impl(self, *args, **kwargs):
        return np.power(args[0], 2, dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np.power({args[0]}, 2)"

    def _c_expr(self, *args, **kwargs):
        return f"square({args[0]}))"


@nargs(2)
@c_headers('<math.h>')
class Pow(Function):

    def _impl(self, *args, **kwargs):
        if any(b < 0 and e < 1 for b, e in np.broadcast(*args)):
            LOG.warning(
                'fractional powers are being passed to a negative base for Pow operator')
        return np.power(args[0], args[1], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return f"np.power({args[0]}, {args[1]})"

    def _c_expr(self, *args, **kwargs):
        return f"powf(({args[0]}), ({args[1]}))"


@nargs(1)
@c_headers('<math.h>')
class Exp(Function):

    def _impl(self, *args, **kwargs):
        return np.exp(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.exp(%s)" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "expf(%s)" % (args[0],)


@nargs(1)
@c_headers('<math.h>')
class TanH(Function):

    def _impl(self, *args, **kwargs):
        return np.tanh(args[0], dtype=self.dtype)

    def _py_expr(self, *args, **kwargs):
        return "np.tanh(%s)" % (args[0],)

    def _c_expr(self, *args, **kwargs):
        return "tanh(%s)" % (args[0],)


@nargs(1)
@c_headers('"float_math_funcs.h"')
@environment({
    'expit': expit,
})
class Sigmoid(Function):

    def _impl(self, *args, **kwargs):
        return expit(args[0]).astype(np.float32)

    def _py_expr(self, *args, **kwargs):
        return f"np.expit({args[0]}).astype(np.float32)"

    def _c_expr(self, *args, **kwargs):
        return f"fsigmoid({args[0]})"


@nargs(1)
@copy_props('cast_dtype')
class Cast(Function):
    def __init__(self, *args, dtype=None, **kwargs):
        self._cast_dtype = dtype
        super().__init__(*args, dtype=dtype, **kwargs)

    @property
    def cast_dtype(self):
        return self._cast_dtype

    def _eval(self, *args, **kwargs):
        if isinstance(args[0], Constant):
            return Constant(args[0].value.astype(self._cast_dtype))
        if args[0].dtype == self._cast_dtype:
            return args[0]
        return self

    def _impl(self, *args, **kwargs):
        return args[0].astype(self._cast_dtype)

    def _py_expr(self, *args, **kwargs):
        return "(%s).astype(np.%s)" % (args[0], self.dtype.__name__)

    def _c_expr(self, *args, **kwargs):
        return "((%s)%s)" % (DTYPE_GAP_CTYPE[self._cast_dtype], args[0])

    def __repr__(self) -> str:
        return "Cast(%s, %s)" % (self.contents[0], self._cast_dtype.__name__)


class CompoundFunction(Function):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._inner_function = self._eval(*args, **kwargs)
        # self._inner_function.name = self.name
        self._inner_function.qrec = self.qrec

    def _collect_globals(self) -> dict:
        global_dict = self.ENVIRONMENT or {}
        global_dict.update(self._inner_function.collect_globals())
        return global_dict

    @property
    def unbound_variables(self):
        return self._inner_function.unbound_variables

    @unbound_variables.setter
    def unbound_variables(self, val):
        pass

    def _resolve(self, **kwargs):
        func = self._inner_function.resolve(**kwargs)
        # func.name = self.name
        func.qrec = self.qrec
        return func

    def _eval(self, *args, **kwargs):
        raise NotImplementedError()

    #pylint: disable=arguments-differ
    def _calculate(self, calculate_ranges=False, **kwargs):
        res = self._inner_function.calculate(
            calculate_ranges=calculate_ranges, **kwargs)
        if calculate_ranges:
            self.control.add_stat(self, res.value)
        return res

    def _impl(self, *args, **kwargs):
        raise ValueError('should not be called')

    def _py_expr(self, *args, **kwargs):
        return self._inner_function.py_expr(*args, **kwargs)

    def _c_expr(self, *args, **kwargs):
        return self._inner_function.c_expr(*args, **kwargs)


@nargs(1)
class HTanh(CompoundFunction):

    def _eval(self, *args, **kwargs):
        return Max(Min(args[0], Constant(1)), Constant(-1))


@nargs(1)
class HSigmoid(CompoundFunction):

    def _eval(self, *args, **kwargs):
        return Mul(Min(Constant(6), Max(Constant(0), Add(args[0], Constant(3), dtype=self.dtype))), Rational(1, 6), dtype=self.dtype)


@nargs(1)
@copy_props('upper_bound', 'lower_bound')
class Relu(CompoundFunction):
    def __init__(self, *args, upper_bound=None, lower_bound=0, **kwargs):
        self._upper_bound = upper_bound
        self._lower_bound = lower_bound
        super().__init__(*args, **kwargs)

    @property
    def upper_bound(self):
        return self._upper_bound

    @property
    def lower_bound(self):
        return self._lower_bound

    def _eval(self, *args, **kwargs):
        if self._upper_bound is not None:
            if self._lower_bound is not None:
                return Min(Constant(self._upper_bound), Max(Constant(self._lower_bound), args[0]))
            else:
                return Min(Constant(self._upper_bound), args[0])
        else:
            if self._lower_bound is not None:
                return Max(Constant(self._lower_bound), args[0])
            else:
                return args[0]


@nargs(1)
@copy_props('_from_qrec', '_to_qrec')
class ConvertFloatScaled(CompoundFunction):
    def __init__(self, *args, from_qrec=None, to_qrec=None, **kwargs):
        self._from_qrec = from_qrec
        self._to_qrec = to_qrec
        super().__init__(*args, **kwargs)

    @property
    def from_qrec(self):
        return self._from_qrec

    @property
    def to_qrec(self):
        return self._to_qrec

    def _eval_float_to_quant(self, *args, **kwargs):
        raise NotImplementedError()

    def _eval_quant_to_float(self, *args, **kwargs):
        raise NotImplementedError()

    def _eval(self, *args, **kwargs):
        if self._from_qrec.dtype == np.int16 or self._from_qrec.dtype == bfloat16:
            return self._eval_float_to_quant(*args, **kwargs)
        return self._eval_quant_to_float(*args, **kwargs)


@nargs(2)
class SquaredDifference(CompoundFunction):
    def _eval(self, *args, **kwargs):
        return Square(Sub(args[0], args[1], dtype=args[0].dtype))
