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
from sympy.core import Mul, Pow, Rational, S
from sympy.core.mul import _keep_coeff
from sympy.printing.precedence import precedence
from sympy.printing.python import PythonPrinter

from .expr_state import ExprState
from .sympy_types import ATLShift, ATNorm, ATScale
from .utils import get_scale_value

LOG = logging.getLogger("nntool." + __name__)

def equalize_scales(var1: ExprState, var2: ExprState, threshold=31-8):
    # if either evaluate to zero then scale to the other. zero is always zero
    if var1.expr.is_zero:
        LOG.debug("equalize - var1 is zero")
        return var1.quant_from(var2), var2
    if var2.expr.is_zero:
        LOG.debug("equalize - var2 is zero")
        return var1, var2.quant_from(var1)
    # if one of the values is not scaled then scale it to the other
    if var1.is_not_scaled:
        LOG.debug("equalize - var1 is not scaled")
        var1.subexpr1 = get_scale_value(var2.scale, var1.expr, dtype=np.int32, q=var2.q, scalar=True)
        return var1.quant_from(var2), var2
    if var2.is_not_scaled:
        LOG.debug("equalize - var2 is not scaled")
        var2.expr = get_scale_value(var1.scale, var2.expr, dtype=np.int32, q=var1.q, scalar=True)
        return var1, var2.quant_from(var1)
    # ajust the q
    if var2.q > var1.q or (var2.length + var1.q - var2.q) > threshold:
        LOG.debug("equalize - adjust q1")
        var1.expr = ATLShift(var1.expr, var2.q - var1.q)
        var1.q = var2.q
    elif var1.q > var2.q or (var1.length + var2.q - var1.q) > threshold:
        LOG.debug("equalize - adjust q2")
        var2.expr = ATLShift(var2.expr, var1.q - var2.q)
        var2.q = var1.q

    # if there is little difference don't scale
    if math.fabs(var1.scale - var2.scale) < 1e-6:
        LOG.debug("equalize - ignore minor scale difference")
        var1.scale = min(var1.scale, var2.scale)
        return var1, var2.scale_from(var1)

    if var1.scale < var2.scale:
        # var2 scale is larger
        # can var2 fit in var1 scale?
        bits_required = math.ceil(math.log2(var2.scale / var1.scale))
        if var2.length + bits_required <= threshold:
            LOG.debug("equalize - rescale var2 to var1")
            scale_var1_to_var2 = False
        else:
            LOG.debug("equalize - overflow: rescale var1 to var2")
            scale_var1_to_var2 = True
    else:
        # var1 scale is larger
        # can var1 fit in var2 scale?
        bits_required = math.ceil(math.log2(var1.scale / var2.scale))
        if var1.length + bits_required <= threshold:
            LOG.debug("equalize - rescale var1 to var2")
            scale_var1_to_var2 = True
        else:
            LOG.debug("equalize - overflow: rescale var2 to var1")
            scale_var1_to_var2 = False

    if scale_var1_to_var2:
        out_var2 = var2
        if var1.scale > var2.scale:
            rounding = math.ceil
        else:
            rounding = math.floor
        bits_adjusted = rounding(math.log2(var1.scale / var2.scale))
        LOG.debug("equalize - rescaling var1 to var2 - bits ajusted by %s", bits_adjusted)
        out_var1 = ExprState(
            ATScale.from_scales(var1.expr, var1.scale, var2.scale, 31-var1.length, to_q=var2.q, from_q=var1.q),
            var1.ibits + bits_adjusted,
            q=var2.q,
            scale=var2.scale
        )
    else:
        out_var1 = var1
        if var2.scale > var1.scale:
            rounding = math.ceil
        else:
            rounding = math.floor
        bits_adjusted = rounding(math.log2(var2.scale / var1.scale))
        LOG.debug("equalize - rescaling var2 to var1 - bits ajusted by %s", bits_adjusted)
        out_var2 = ExprState(
            ATScale.from_scales(var2.expr, var2.scale, var1.scale, 31-var2.length, to_q=var1.q, from_q=var2.q),
            var2.ibits + bits_adjusted,
            q=var1.q,
            scale=var1.scale
        )

    return out_var1, out_var2

def from_node(G, node, idx=None):
    if idx is None:
        return G.in_edges(node.name)[0].from_node
    return next(edge.from_node for edge in G.in_edges(node.name) if edge.to_idx == idx)


def check_overflow(var, extra_bits, threshold=31-8):
    bits_used = var.length + extra_bits
    if bits_used <= threshold:
        return var
    needed = bits_used - threshold
    LOG.debug("check overflow - reclaim %s bits", needed)
    var.scale *= pow(2, needed)
    var.expr = ATNorm(var.expr, needed)
    var.length -= needed
    return var


def check_mul_overflow(var1, var2, threshold=31-8):
    # if this is a whole number then just the scale is modified
    if var1.is_not_scaled:
        LOG.debug("check muloverflow - var1 not scaled")
        return var1.set_to_constant(var1.expr), var2
    if var2.is_not_scaled:
        LOG.debug("check muloverflow - var2 not scaled")
        return var1, var2.set_to_constant(var2.expr)

    bits_used = var1.length + var2.length
    if bits_used <= threshold:
        return var1, var2

    needed = bits_used - threshold
    LOG.debug("check muloverflow - reclaim %s bits", needed)

    var_arr = [var1, var2]
    old_length = [var1.length, var2.length]
    new_length = [var1.length, var2.length]

    if var1.length > var2.length:
        take_bits = min(var1.length - var2.length, needed)
        new_length[0] -= take_bits
        needed -= take_bits
    elif var2.length > var1.length:
        take_bits = min(var2.length - var1.length, needed)
        new_length[1] -= take_bits
        needed -= take_bits
    if needed > 0:
        take_bits1 = needed // 2
        take_bits2 = needed - take_bits1
        new_length[0] -= take_bits1
        new_length[1] -= take_bits2

    for i in range(2):
        if old_length[i] == new_length[i]:
            continue
        LOG.debug("check muloverflow - var%s old %s new %s", i, old_length[i], new_length[i])
        needed = old_length[i] - new_length[i]
        var_arr[i].length -= needed
        var_arr[i].scale *= pow(2, needed)
        var_arr[i].expr = ATNorm(var_arr[i].expr, needed)

    return var1, var2

class PythonPrinterEx(PythonPrinter):
    def __init__(self, settings=None):
        super(PythonPrinterEx, self).__init__(settings)
        self.symbols = []
        self.functions = []

        # Create print methods for classes that should use StrPrinter instead
        # of ReprPrinter.
        for ex_funcs in [("_print_Mul", "_print_Mul_ex"), ("_print_Pow", "_print_Pow_ex")]:
            setattr(PythonPrinterEx, ex_funcs[0], getattr(PythonPrinterEx, ex_funcs[1]))

    def doprint(self, expr):
        res = super(PythonPrinterEx, self).doprint(expr)
        return res

    def _print_Mul_ex(self, expr):

        prec = precedence(expr)

        c, e = expr.as_coeff_Mul()
        if c < 0:
            expr = _keep_coeff(-c, e)
            sign = "-"
        else:
            sign = ""

        a = []  # items in the numerator
        b = []  # items that are in the denominator (if any)

        pow_paren = []  # Will collect all pow with more than one base element and exp = -1

        if self.order not in ('old', 'none'):
            args = expr.as_ordered_factors()
        else:
            # use make_args in case expr was something like -x -> x
            args = Mul.make_args(expr)

        # Gather args for numerator/denominator
        for item in args:
            if item.is_commutative and item.is_Pow and item.exp.is_Rational and item.exp.is_negative:
                if item.exp != -1:
                    b.append(Pow(item.base, -item.exp, evaluate=False))
                else:
                    if len(item.args[0].args) != 1 and isinstance(item.base, Mul):   # To avoid situations like #14160
                        pow_paren.append(item)
                    b.append(Pow(item.base, -item.exp))
            elif item.is_Rational and item is not S.Infinity:
                if item.p != 1:
                    a.append(Rational(item.p))
                if item.q != 1:
                    b.append(Rational(item.q))
            else:
                a.append(item)

        a = a or [S.One]

        a_str = [self.parenthesize(x, prec, strict=False) for x in a]
        b_str = [self.parenthesize(x, prec, strict=False) for x in b]

        # To parenthesize Pow with exp = -1 and having more than one Symbol
        for item in pow_paren:
            if item.base in b:
                b_str[b.index(item.base)] = "(%s)" % b_str[b.index(item.base)]

        if not b:
            return sign + '*'.join(a_str)
        elif len(b) == 1:
            return sign + '*'.join(a_str) + "//" + b_str[0]
        else:
            return sign + '*'.join(a_str) + "//(%s)" % '*'.join(b_str)

    def _print_Pow_ex(self, expr, rational=False):
        PREC = precedence(expr)

        if expr.exp is S.Half and not rational:
            return "sqrt(%s)" % self._print(expr.base)

        if expr.is_commutative:
            if -expr.exp is S.Half and not rational:
                # Note: Don't test "expr.exp == -S.Half" here, because that will
                # match -0.5, which we don't want.
                return "%s/sqrt(%s)" % tuple(map(self._print, (S.One, expr.base)))
            if expr.exp is -S.One:
                # Similarly to the S.Half case, don't test with "==" here.
                return '%s/%s' % (self._print(S.One),
                                  self.parenthesize(expr.base, PREC, strict=False))

        e = self.parenthesize(expr.exp, PREC, strict=False)
        # TODO - deal with rational here
        if self.printmethod == '_sympyrepr' and expr.exp.is_Rational and expr.exp.q != 1:
            # the parenthesized exp should be '(Rational(a, b))' so strip parens,
            # but just check to be sure.
            if e.startswith('(Rational'):
                return 'Pow(%s, %s)' % (self.parenthesize(expr.base, PREC, strict=False), e[1:-1])
        return 'Pow(%s, %s)' % (self.parenthesize(expr.base, PREC, strict=False), e)
