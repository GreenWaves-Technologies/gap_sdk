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


import numpy as np

from .symbol import Constant, Symbol, Variable, environment


@environment({'np': np})
class Function(Symbol):

    def __init__(self, *args, **kwargs):
        super(Function, self).__init__(*args, **kwargs)
        self.unbound_variables = self._init_unbound_variables(*args)
        self._q = None

    @property
    def q(self):
        return self._q

    @property
    def unbound_variables(self):
        return self._unbound_variables

    @unbound_variables.setter
    def unbound_variables(self, val):
        self._unbound_variables = val

    @property
    def variable_quantization(self):
        qrecs = {}
        for sym in self._contents:
            if isinstance(sym, (Variable, Function)):
                qrecs.update(sym.variable_quantization)
        return qrecs

    @property
    def ops(self):
        ops = sum(sym.ops for sym in self._contents)
        return ops + 1

    @staticmethod
    def _init_unbound_variables(*args):
        unbound_variables = {}
        for arg in args:
            if isinstance(arg, Variable):
                if arg.name in unbound_variables:
                    if unbound_variables[arg.name] != arg:
                        raise ValueError('there is more than one variable called %s' % arg.name)
                else:
                    unbound_variables[arg.name] = arg
            elif isinstance(arg, Function):
                unbound_variables.update(arg.unbound_variables)
            elif isinstance(arg, str):
                if arg in unbound_variables:
                    raise ValueError('there is more than one variable called %s' % arg)
                else:
                    unbound_variables[arg] = Variable(arg)
                
        return unbound_variables

    def _eval(self, *args, **kwargs):
        if all(isinstance(arg, Constant) for arg in args):
            val = self._impl(*[arg.value for arg in args])
            if val.dtype != self.dtype:
                raise ArithmeticError("Expression evaluated to incorrect dtype")
            return Constant(val,
                            dtype=val.dtype,
                            shape=val.shape,
                            name=self.name,
                            qrec=self.qrec,
                            **kwargs)
        return self

    @property
    def is_constant(self):
        return all(sym.is_constant for sym in self._contents)

    def _impl(self, *args, **kwargs):
        raise NotImplementedError('_impl has not been overriden')

    def _resolve(self, **kwargs):
        contents = [elem.resolve(**kwargs)
                    for elem in self._contents]
        self._contents = contents
        self.unbound_variables = self._init_unbound_variables(*self._contents)
        return self._eval(*self._contents, **kwargs)

    def _calculate(self, calculate_ranges=False, track_results=None, **kwargs):
        contents = [elem.calculate(calculate_ranges=calculate_ranges,
                                   track_results=track_results,
                                   **kwargs)
                    for elem in self._contents]
        res = self._eval(*contents, **kwargs)
        if calculate_ranges:
            self.control.add_stat(self, res.value)
        if track_results is not None:
            if self.qrec is not None:
                track_results[self.name] = self.qrec.dequantize(res.value.copy())
            else:
                track_results[self.name] = res.value.copy()
        return res

    def __call__(self, *args, substitute=False, **kwargs):
        val = self._resolve(
            **kwargs) if substitute else self._calculate(**kwargs)
        if isinstance(val, Constant):
            return val.value
        return val

    def call_with_constants(self, *args):
        return self._impl(*args)

    @property
    def shape(self):
        if not self._shape:
            self._shape = self.broadcast(*(elem.shape for elem in self._contents))
        return self._shape

    @property
    def py_expr(self) -> str:
        return self._py_expr(*(arg.py_expr for arg in self._contents))

    def _collect_globals(self) -> dict:
        global_dict = self.ENVIRONMENT or {}
        for elem in self._contents:
            global_dict.update(elem.collect_globals())
        return global_dict

    @property
    def c_expr_func(self):
        return self._c_expr

    @property
    def py_compiled_lambda(self):
        global_dict = self.collect_globals()
#pylint: disable=eval-used
        return eval(self.py_lambda, global_dict)

    def c_expr(self, *args, **kwargs) -> str:
        return self._c_expr(*(arg.c_expr(*args, **kwargs) for arg in self._contents))

    @property
    def py_lambda(self) -> str:
        return "lambda %s: %s" % (",".join("%s=None" % (var) for var in self.unbound_variables), self.py_expr)

    def c_assignment(self, var_name) -> str:
        return "%s=%s;" % (var_name, self.c_expr())
