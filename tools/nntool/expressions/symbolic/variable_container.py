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

from abc import ABC, abstractmethod, abstractproperty
from typing import Mapping

import numpy as np
from utils.disjoint_reduction import disjoint_reduction

from .symbol import Symbol, Variable


class VariableAssigner(ABC):
    @abstractmethod
    def _resolve_assignment(self, substitute_all=False, **kwargs) -> Mapping[str, Symbol]:
        """ Resolves an container that is one or more assigments substituting values contained in
        **kwargs into unresolved variables

        Args:
            substitute_all (bool, optional): If False only expressions that resolve to constants will be substituted.
                                             Defaults to False.

        Returns:
            Mapping[str, Symbol]: A map of the variable names and their values (Symbols)
        """

    def resolve_assignment(self, substitute_all=False, **kwargs) -> Mapping[str, Symbol]:
        return self._resolve_assignment(substitute_all=substitute_all, **kwargs)

    @abstractmethod
    def _calculate_assignment(self, **kwargs) -> Mapping[str, np.ndarray]:
        """ Attempts to resolve a series of assignments to a map of values

        Returns:
            Mapping[str, np.ndarray]: Map of resolved values
        """

    def calculate_assignment(self, **kwargs) -> Mapping[str, np.ndarray]:
        return self._calculate_assignment(**kwargs)

    @abstractproperty
    def returned_variables(self):
        pass

    @abstractproperty
    def var_shapes(self):
        pass

class VariableContainer():
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._unbound_variables = self._init_unbound_variables(*args)

    @property
    def unbound_variables(self):
        return self._unbound_variables

    @unbound_variables.setter
    def unbound_variables(self, val):
        self._unbound_variables = val

    @property
    def unbound_shapes(self):
        return (var.shape
                for var in self.unbound_variables.values())

    @property
    def extended_unbound_var_shapes(self):
        max_length = max(len(var.shape)
                         for var in self.unbound_variables.values())
        return {vname: tuple(([1] * (max_length - len(var.shape))) + list(var.shape))
                for vname, var in self.unbound_variables.items()}


    @staticmethod
    def adjust(axes, adjust):
        return tuple(tuple(dim+adjust for dim in axes_group) for axes_group in axes)


    @property
    def axes(self):
        var_shapes = Symbol.extend_shapes(*self.unbound_shapes)
        axes = disjoint_reduction(set(frozenset(idx for idx, dim in enumerate(
            shape) if dim != 1) for shape in var_shapes))
        return tuple(sorted([tuple(x) for x in axes]))

    @staticmethod
    def _init_unbound_variables(*args):
        unbound_variables = {}
        for arg in args:
            if isinstance(arg, Variable):
                if arg.name in unbound_variables:
                    if unbound_variables[arg.name].shape != arg.shape:
                        raise ValueError(
                            'there is more than one variable called %s with different shapes' % arg.name)
                else:
                    unbound_variables[arg.name] = arg
            elif isinstance(arg, VariableContainer):
                unbound_variables.update(arg.unbound_variables)
            elif isinstance(arg, str):
                if arg in unbound_variables:
                    raise ValueError(
                        'there is more than one variable called %s' % arg)
                else:
                    unbound_variables[arg] = Variable(arg)

        return unbound_variables

    def vars_to_axes(self, axes=None):
        if axes is None:
            axes = self.axes
        return {vname: tuple(axis for axis in axes if all(shape[dim] > 1 for dim in axis))
                for vname, shape in self.extended_unbound_var_shapes.items()}

    def axes_sizes(self, axes=None):
        if axes is None:
            axes = self.axes
        shape = Symbol.broadcast(*self.unbound_shapes)
        return {axis: int(np.prod([shape[x] for x in axis])) for axis in axes}


class VariableContainerAndAssigner(VariableContainer, VariableAssigner):
    @property
    def var_axes(self):
        elems = self.resolve_assignment(substitute_all=True)
        max_axis_groups = np.array(
            [max(max(x) for x in elem.axes) for elem in elems.values()])
        max_axis = np.max(max_axis_groups)
        axis_adjust = max_axis - max_axis_groups

        axes = {}
        for elem_idx, (elem_name, elem) in enumerate(elems.items()):
            axes[elem_name] = self.adjust(elem.axes, axis_adjust[elem_idx])
            for vname, vaxes in elem.vars_to_axes().items():
                axes[vname] = self.adjust(vaxes, axis_adjust[elem_idx])
        return axes
