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
import math

import numpy as np
from bfloat16 import bfloat16
from quantization.qtype import DTYPE_GAP_CTYPE
from scipy.special import expit

from .function import Function
from .symbol import (Constant, QRecBase, Rational, Symbol, Variable, c_headers,
                     copy_props, environment, handles, handlesr, nargs)

LOG = logging.getLogger('nntool.'+__name__)


@nargs(2)
@handles('__add__')
@handlesr('__radd__')
class Add(Function):

    def _impl(self, *args, **kwargs):
        res = np.add(args[0], args[1], dtype=self.dtype)
        return res

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
        self._inner_function.tag = self.tag
        self._inner_function.comment = self.comment

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
        if isinstance(func, Function):
            func.tag = self.tag
            func.comment = self.comment
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

    def c_block(self, code_block=None, tags=None, **kwargs):
        if tags is not None and self._inner_function not in tags:
            name = tags.get(self, f'{self.SYMBOL_PREFEX}{self.name}')
            if isinstance(name, str):
                name = (Variable(name, dtype=self.dtype), True)
            tags[self._inner_function] = name
        return self._inner_function.c_block(code_block=code_block, tags=tags, **kwargs)


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


@nargs(3)
class ClipFloat(CompoundFunction):

    def _eval(self, *args, **kwargs):
        return Min(Max(args[0], args[1], dtype=self.dtype), args[2], dtype=self.dtype)


@nargs(1)
@copy_props('_from_qrec', '_to_qrec')
class ConvertQuantization(CompoundFunction):
    def __init__(self, *args, from_qrec: QRecBase=None, to_qrec: QRecBase=None, **kwargs):
        self._from_qrec = from_qrec
        self._to_qrec = to_qrec
        super().__init__(*args, **kwargs)

    @property
    def from_qrec(self) -> QRecBase:
        return self._from_qrec

    @property
    def from_is_float(self) -> bool:
        return self._from_qrec.dtype in [np.float16, np.float32, bfloat16]

    @property
    def from_is_fix(self) -> bool:
        return self._from_qrec.dtype in [np.int8, np.uint8, np.int16, np.uint16, np.int32]

    @property
    def to_is_float(self) -> bool:
        return self._to_qrec.dtype in [np.float16, np.float32, bfloat16]

    @property
    def to_is_fix(self) -> bool:
        return self._to_qrec.dtype in [np.int8, np.uint8, np.int16, np.uint16, np.int32]

    @property
    def to_qrec(self) -> QRecBase:
        return self._to_qrec

    def _eval_float_to_fix(self, *args, **kwargs) -> Symbol:
        to_qrec = self.to_qrec
        from_qrec = self.from_qrec
        scaled_val = Mul(
            args[0],
            Constant(
                [math.pow(2, to_qrec.q)/to_qrec.scale],
                dtype=from_qrec.dtype),
            dtype=from_qrec.dtype)
        if to_qrec.zero_point != 0:
            # need to add zero_point plus rounding
            scaled_val = Add(
                scaled_val,
                Constant([to_qrec.zero_point + 0.5], dtype=from_qrec.dtype),
                dtype=from_qrec.dtype)
        else:
            # Just add rounding
            scaled_val = Add(
                scaled_val,
                Constant([0.5], dtype=from_qrec.dtype),
                dtype=from_qrec.dtype)
        iinfo = np.iinfo(to_qrec.dtype)
        return Cast(
            ClipFloat(
                scaled_val,
                Constant(iinfo.min, dtype=from_qrec.dtype),
                Constant(iinfo.max, dtype=from_qrec.dtype),
                dtype=from_qrec.dtype),
            dtype=to_qrec.dtype,
            tag=self.tag,
            comment=self.comment)

    def _eval_fix_to_float(self, *args, **kwargs) -> Symbol:
        to_qrec = self.to_qrec
        from_qrec = self.from_qrec
        float_val = Cast(args[0], dtype=to_qrec.dtype)
        if from_qrec.zero_point != 0:
            float_val = Sub(
                float_val,
                Constant([from_qrec.zero_point], dtype=to_qrec.dtype),
                dtype=to_qrec.dtype)
        float_val = Mul(
            float_val,
            Constant(
                [from_qrec.scale/math.pow(2, from_qrec.q)],
                dtype=to_qrec.dtype),
            dtype=to_qrec.dtype,
            tag=self.tag,
            comment=self.comment)
        return float_val

    def _eval(self, *args, **kwargs) -> Symbol:
        if self.from_is_float:
            if self.to_is_fix:
                return self._eval_float_to_fix(*args, **kwargs)
            elif self.to_is_float:
                if self.to_qrec.dtype != self.from_qrec.dtype:
                    return Cast(
                        *args,
                        dtype=self.to_qrec.dtype,
                        **kwargs)
                return args[0]
        elif self.from_is_fix:
            if self.to_is_float:
                return self._eval_fix_to_float(*args, **kwargs)
            elif self.to_is_fix:
                # if self.to_qrec.dtype == self.from_qrec.dtype:
                #     return args[0]
                # sign_change = from_qrec.signed != to_qrec.signed
                # growing = from_qrec.size < to_qrec.size
                # reducing = from_qrec.size > to_qrec.size
                # zeropoint_change = from_qrec.zero_point != to_qrec.zero_point
                # scale_change = from_qrec.scale != to_qrec.scale
                # q_change = from_qrec.q != to_qrec.q
                raise NotImplementedError()

        raise ValueError('unsupported conversion')


@nargs(2)
class SquaredDifference(CompoundFunction):
    def _eval(self, *args, **kwargs):
        return Square(Sub(args[0], args[1], dtype=args[0].dtype))
