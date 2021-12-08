# Copyright (C) 2021  GreenWaves Technologies, SAS

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


from functools import reduce
from typing import Mapping

import numpy as np
from generation.code_block import CodeBlock

from .iteration_space import IterationSpace
from .symbol import Constant, Symbol, Variable, copy_props
from .variable_container import VariableContainerAndAssigner


@copy_props('var')
class Assignment(VariableContainerAndAssigner, Symbol):
    def __init__(self, arg, name="", var=None, **kwargs):
        if var is None:
            self._var = Variable(name, shape=arg.shape, symbol_binding=arg)
        else:
            name = var.name
            self._var = var
        super(Assignment, self).__init__(arg, name=name, **kwargs)

    @property
    def unbound_variables(self):
        return self.contents[0].unbound_variables

    @property
    def var(self):
        return self._var

    def find(self, name):
        for elem in [self._var, self.contents[0]]:
            res = elem.find(name)
            if res:
                return res
        return None

    @property
    def var_shapes(self):
        shapes = {self.name: self.contents[0].shape}
        shapes.update(zip(self.contents[0].unbound_variables, self.contents[0].unbound_shapes))
        return shapes

    def _resolve(self, **kwargs):
        """Given a set of substitions for variable in kwargs resolve all variables"""
        return self._contents[0].resolve(**kwargs)

    def _resolve_assignment(self, substitute_all=False, **kwargs) -> Mapping[str, Symbol]:
        return {self.name: self._contents[0].resolve(**kwargs)}

    def _calculate(self, calculate_ranges=False, **kwargs):
        res = self._contents[0].resolve(**kwargs)
        if not isinstance(res, Constant):
            raise ValueError(
                f"unable to calculate {self.name}")
        if calculate_ranges:
            self.control.add_stat(self, res.value)
        return res.value

    def _calculate_assignment(self, **kwargs) -> Mapping[str, np.ndarray]:
        return {self.name: self._calculate(**kwargs)}

    def c_block(self, code_block: CodeBlock = None, iteration_space: IterationSpace = None):
        if code_block is None:
            code_block = CodeBlock()
        if iteration_space:
            if self.var.name in iteration_space.temporary_variables:
                code_block.write(
                    f"{self.var.c_expr(declare=True, dtype=self.contents[0].dtype)}"
                    f" = {self.contents[0].c_expr(iteration_space=iteration_space)};")
            else:
                code_block.write(
                    f"{self.var.c_expr(dtype=self.contents[0].dtype)}{iteration_space.c_index(self.var.name)}"
                    f" = {self.contents[0].c_expr(iteration_space=iteration_space)};")
        else:
            code_block.write(f'{self.var.name} = {self.contents[0].c_expr()};')
        return code_block

    @property
    def returned_variables(self):
        return [self.name]

    @property
    def shape(self):
        return self._contents[0].shape

    def _py_expr(self, *args, **kwargs):
        return self._contents[0].py_expr(*args, **kwargs)

    def _c_expr(self, *args, **kwargs):
        return self._contents[0].c_expr(*args, **kwargs)

    def __repr__(self) -> str:
        return f"{{{self.var.name} <- {self.contents[0].__repr__()}}}"


@copy_props('preconditions', 'returned_variables')
class Let(VariableContainerAndAssigner, Symbol):
    def __init__(self, *args, preconditions=None, returned_variables=None, name="", **kwargs):
        args = [Assignment(arg[1], name=arg[0]) if isinstance(
            arg, tuple) else arg for arg in args]
        super(Let, self).__init__(*args, name=name, **kwargs)
        if preconditions is None:
            preconditions = []
        else:
            preconditions = [Assignment(arg[1], name=arg[0]) if isinstance(
                arg, tuple) else arg for arg in preconditions]
        self._preconditions = preconditions
        self._returned_variables = returned_variables

# pylint: disable=invalid-name
    def In(self, *expressions):
        return Let(*expressions, preconditions=[self])

    def Return(self, *variable_names):
        produced = self.produced_variables
        if not all(variable in produced for variable in variable_names):
            raise ValueError('not all variables are produced')
        return Let(*self.contents, preconditions=self.preconditions, name=self.name, returned_variables=variable_names)

    @property
    def unbound_variables(self):
        resolution = self.resolve_assignment()
        _vars = reduce(lambda s, x: s | set(
            x.unbound_variables.values()), resolution.values(), set())
        return {var.name: var for var in _vars if var.name not in set(resolution.keys())}

    @property
    def produced_variables(self):
        resolution = self.resolve_assignment()
        return set(resolution.keys())

    @property
    def preconditions(self):
        return self._preconditions

    @property
    def returned_variables(self):
        return self._returned_variables

    @staticmethod
    def substitute_variables(assignments):
        res = {}
        substitutions = {}
        for var_name, val in assignments.items():
            if isinstance(val, (Constant, np.ndarray, int, float)):
                substitutions[var_name] = val
            else:
                substitutions[var_name] = Variable(var_name, shape=val.shape)
                res[var_name] = val
        return res, substitutions

    def find(self, name):
        for elem in list(self._preconditions) + list(self.contents):
            res = elem.find(name)
            if res:
                return res
        return None

    def _resolve_assignment(self, substitute_all=False, **kwargs) -> Mapping[str, Symbol]:
        """Given a set of substitions for variable in kwargs resolve all variables - return a dictionary of variables"""
        preconditions = self._resolve_contents(
            contents=self._preconditions, substitute_all=substitute_all, **kwargs)
        return self._resolve_contents(contents=self.contents, substitute_all=substitute_all, **preconditions)

    def _calculate_assignment(self, **kwargs) -> Mapping[str, np.ndarray]:
        preconditions = self._calculate_contents(
            contents=self._preconditions, **kwargs)
        res = self._calculate_contents(contents=self.contents, **preconditions)
        if self.returned_variables:
            res = {vname: val for vname, val in res.items(
            ) if vname in self.returned_variables}
        return res

    @staticmethod
    def _resolve_contents(contents=None, substitute_all=False, **kwargs):
        if substitute_all:
            substitutions = kwargs
            res = kwargs
        else:
            res, substitutions = Let.substitute_variables(kwargs)
        for elem in contents:
            elem_res = elem.resolve_assignment(
                substitute_all=substitute_all, **substitutions)
            if substitute_all:
                substitutions.update(elem_res)
                res.update(elem_res)
            else:
                elem_res, elem_substitutions = Let.substitute_variables(
                    elem_res)
                res.update(elem_res)
                substitutions.update(elem_substitutions)
        return res

    @staticmethod
    def _calculate_contents(contents=None, **kwargs):
        for elem in contents:
            kwargs.update(elem.calculate_assignment(**kwargs))
        return kwargs

    def _resolve(self, **kwargs):
        """Given a set of substitions for variable in kwargs resolve all variables - return a single symbol"""
        preconditions = self._resolve_contents(
            contents=self._preconditions, substitute_all=True, **kwargs)
        resolution = self._resolve_contents(
            contents=self.contents, substitute_all=True, **preconditions)
        return Assignment(resolution[self.contents[-1].name], name=self.contents[-1].name)

    def _calculate(self, calculate_ranges=False, **kwargs):
        res = self._resolve(**kwargs)
        if not isinstance(res.contents[0], Constant):
            raise ValueError(
                f"unable to calculate {self.name}")
        if calculate_ranges:
            self.control.add_stat(self, res.value)
        return res.contents[0].value

    @property
    def var_shapes(self):
        shapes = {}
        for var_name, elem in self.resolve_assignment().items():
            shapes[var_name] = elem.shape
            shapes.update(dict(zip(elem.unbound_variables, elem.unbound_shapes)))
        return shapes

    @property
    def shape(self):
        return self._contents[-1].shape

    def _py_expr(self, *args, **kwargs):
        return self._contents[0].py_expr(*args, **kwargs)

    def c_block(self, code_block: CodeBlock = None, iteration_space: IterationSpace = None, with_loops=False):
        if code_block is None:
            code_block = CodeBlock()
        if with_loops:
            assert iteration_space, "must have space"
            for idx, _ in enumerate(iteration_space.axis_shape):
                if idx in iteration_space.fixed_spaces:
                    continue
                code_block.write(f"{iteration_space.c_for(idx)} {{")
                code_block.indent()
        for precondition in self.preconditions:
            precondition.c_block(code_block=code_block,
                                 iteration_space=iteration_space)
        for item in self.contents:
            item.c_block(code_block=code_block,
                         iteration_space=iteration_space)
        if with_loops:
            for idx, _ in enumerate(iteration_space.axis_shape):
                if idx in iteration_space.fixed_spaces:
                    continue
                code_block.deindent()
                code_block.write("}")
        return code_block

    def _c_expr(self, *args, **kwargs):
        return self._contents[0].c_expr(*args, **kwargs)

    def __repr__(self) -> str:
        return (f"Let({','.join([elem.__repr__() for elem in self.preconditions])})"
                f".In({','.join([elem.__repr__() for elem in self.contents])})")
