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

import numpy as np
from sympy import Symbol
from sympy.utilities.lambdify import lambdastr, lambdify
from utils.json_serializable import JsonSerializable

from utils.pow_sqrt import logn_17_15, pow_17_15, sqrt_17_15

from .expr_state import ExprState
from .misc import PythonPrinterEx

LOG = logging.getLogger("nntool." + __name__)


def array_min(*args):
    return reduce(np.minimum, args)


def array_max(*args):
    return reduce(np.maximum, args)


LAMBDA_MODULES_FLOAT = {'Min': array_min, 'Max': array_max, 'sqrt': np.sqrt, 'log': np.log}

LAMBDA_MODULES_QUANT = {'Min': array_min, 'Max': array_max,
                        'sqrt': sqrt_17_15, 'log': logn_17_15, 'Pow': pow_17_15}


class Expression(JsonSerializable):
    def __init__(self, name, var: ExprState, is_quant=False, is_intermediate=False):
        self._name = name
        self._var = var
        self._is_quant = is_quant
        self._is_intermediate = is_intermediate

    def _encapsulate(self):
        return {
            'name': self._name,
            'var': self._var,
            'is_quant': self._is_quant,
            'is_intermediate': self._is_intermediate
        }

    @classmethod
    def _dencapsulate(cls, val):
        return cls(val['name'], val['var'], is_quant=val['is_quant'], is_intermediate=val['is_intermediate'])

    @property
    def expr(self):
        return self._var.expr

    @expr.setter
    def expr(self, val):
        self._var.expr = val

    @property
    def symbol(self):
        return Symbol(self._name)

    @property
    def is_intermediate(self):
        return self._is_intermediate

    @property
    def symbolic_var(self):
        return ExprState(Symbol(self._name), self._var.ibits, q=self._var.q, scale=self._var.scale)

    @property
    def num_bits(self):
        return self._var.length

    def lambdastr(self):
        printer = PythonPrinterEx if self._is_quant else None
        return lambdastr(self.expr.free_symbols, self.expr, printer=printer)

    def execute(self, subs, use_imps=True):
        printer = PythonPrinterEx if self._is_quant else None
        lambda_modules = LAMBDA_MODULES_QUANT if self._is_quant else LAMBDA_MODULES_FLOAT
        func = lambdify(self.expr.free_symbols, self.expr,
                        modules=lambda_modules, use_imps=use_imps, printer=printer)
        inputs = [subs[symbol.name] for symbol in self.expr.free_symbols]
        return func(*inputs)

    def __str__(self):
        if self._var.scale is None:
            return "(%s bits) %s=%s" % (self.num_bits, self._name, self.lambdastr())
        return "(%s.%s %s) %s=%s" % (self._var.ibits, self._var.q, self._var.scale, self._name, self.lambdastr())
