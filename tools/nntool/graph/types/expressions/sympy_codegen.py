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
from functools import reduce

from sympy import Range
from sympy.codegen.ast import (Assignment, Declaration, For, Pointer, Token,
                               Type, Variable, pointer_const, untyped,
                               value_const)
from sympy.printing.ccode import C99CodePrinter
from sympy.printing.precedence import precedence
from sympy.tensor.indexed import Indexed

LOG = logging.getLogger("nntool." + __name__)

VOID = Type("void")


#pylint: disable=no-init
class AssignmentEx(Assignment):
    @classmethod
    def _check_args(cls, lhs, rhs):
        if isinstance(lhs, Declaration):
            super(AssignmentEx, cls)._check_args(lhs.variable, rhs)
        else:
            super(AssignmentEx, cls)._check_args(lhs, rhs)


#pylint: disable=no-init
class IndexedEx(Indexed):
    def _sympystr(self, p):
        indices = list(map(p.doprint, self.indices))
        shape_size = reduce(lambda s, x: ["%s*%s" % (x, s[0])] + s,
                            [shape for shape in self.base.shape[-1:0:-1]], ["1"])
        indices = "+".join(indices[idx] if shape_size[idx] == "1" else "%s*(%s)" %
                           (indices[idx], shape_size[idx]) for idx in range(len(indices)))
        return "%s[%s]" % (p.doprint(self.base), indices)

#pylint: disable=no-init
class StructMember(Token):
    __slots__ = ['variable']


#pylint: disable=no-init
class Struct(Type):
    __slots__ = Type.__slots__ + ['members']


#pylint: disable=no-init
class TypeDef(Token):
    __slots__ = ["symbol", "type"]


#pylint: disable=no-init
class StructVariable(Variable):
    __slots__ = ['parent'] + Variable.__slots__


#pylint: disable=no-init
class VarRange(Token):
    __slots__ = ["start", "stop", "step"]


class VarFor(For):
    @classmethod
#pylint: disable=invalid-name
    def _construct_iterable(cls, itr):
        if isinstance(itr, VarRange):
            return itr
        return super(VarFor)._construct_iterable(itr)

class CPrinter(C99CodePrinter):

#pylint: disable=invalid-name
    def _print_Pow(self, expr):
        prec = precedence(expr)

        if expr.exp == -1:
            LOG.warning("reciprocal in function - probably incorrect")
            return '1/%s' % (self.parenthesize(expr.base, prec))
        elif expr.exp == 0.5:
            return '%s(%s)' % (self.known_functions['sqrt'], self._print(expr.base))
        else:
            return '%s(%s, %s)' % (self.known_functions['Pow'], self._print(expr.base),
                                   self._print(expr.exp))

#pylint: disable=invalid-name
    def _print_For(self, expr):
        target = self._print(expr.target)
        if isinstance(expr.target, Declaration):
            itervar = self._print(expr.target.variable)
        else:
            itervar = target

        if isinstance(expr.iterable, Range):
            start, stop, step = expr.iterable.args
        elif isinstance(expr.iterable, VarRange):
            start, stop, step = tuple(self._print(arg) if isinstance(arg, Variable) else arg
                                      for arg in expr.iterable.args)
        else:
            raise NotImplementedError("Only iterable currently supported is Range")
        body = self._print(expr.body)
        if step.is_number and step == 1:
            inc = "{itervar}++".format(itervar=itervar)
        else:
            inc = "{itervar} += {step}".format(itervar=itervar, step=step)
        return ('for ({target} = {start}; {itervar} < {stop}; {inc}) {{\n{body}\n}}').format(
            target=target, start=start,
            stop=stop, body=body, itervar=itervar, inc=inc)

    _print_VarFor = _print_For

#pylint: disable=invalid-name
    def _print_TypeDef(self, expr):
        if isinstance(expr.type, Struct):
            return "typedef struct {{\n{members}\n}} {symbol}".format(
                symbol=self._print(expr.symbol),
                members="\n".join("{type} {pointer}{name};".format(
                    type=self._print(member.variable.type),
                    pointer="*" if isinstance(member.variable, Pointer) else "",
                    name=member.variable.symbol.name) for member in expr.type.members)
            )
        else:
            return "typedef {type} {symbol}".format(
                type=self._print(expr.type),
                symbol=self._print(expr.symbol)
            )

#pylint: disable=invalid-name
    def _print_StructVariable(self, expr):
        if isinstance(expr.parent, Pointer):
            return "{parent}->{symbol}".format(parent=self._print(expr.parent), symbol=expr.symbol.name)
        return "{parent}.{symbol}".format(parent=self._print(expr.parent), symbol=expr.symbol.name)

    def _print_Declaration(self, decl):
#pylint: disable=import-outside-toplevel
        from sympy.codegen.cnodes import restrict
        var = decl.variable
        val = var.value
        if var.type == untyped:
            raise ValueError("C does not support untyped variables")

        if isinstance(var, Pointer):
            result = '{vc}{t} *{pc} {r}{s}'.format(
                vc='const ' if value_const in var.attrs else '',
                t=self._print(var.type),
                pc=' const' if pointer_const in var.attrs else '',
                r='__restrict__ ' if restrict in var.attrs else '',
                s=var.symbol.name
            )
        elif isinstance(var, Variable):
            result = '{vc}{t} {s}'.format(
                vc='const ' if value_const in var.attrs else '',
                t=self._print(var.type),
                s=self._print(var.symbol)
            )
        else:
            raise NotImplementedError("Unknown type of var: %s" % type(var))
        if val != None:  # Must be "!= None", cannot be "is not None"
            result += ' = %s' % self._print(val)
        return result


def ccode(expr, assign_to=None, **settings):
    return CPrinter(settings).doprint(expr, assign_to)
