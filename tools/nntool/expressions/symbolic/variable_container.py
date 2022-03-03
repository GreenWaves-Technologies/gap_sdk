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

from collections import Counter
from itertools import chain

import numpy as np
from utils.disjoint_reduction import disjoint_reduction

from .symbol import Symbol, Variable


def search_variables(elem):
    if isinstance(elem, Variable):
        return [elem]
    if type(elem) == int or type(elem) == float or isinstance(elem, np.ndarray):
        return []
    return chain(*[search_variables(sub_elem) for sub_elem in elem.contents])

class VariableContainer():
    def __init__(self, *args, **kwargs):
        args = list(args)
        # if string variable names are provided match existing variables or create a new one
        if any(isinstance(arg, str) for arg in args):
            all_vars = {var.name: var
                        for var in chain(*[search_variables(arg)
                                        for arg in args if not isinstance(arg, str)])}
            for idx, arg in enumerate(args):
                if isinstance(arg, str):
                    if arg in all_vars:
                        args[idx] = all_vars[arg]
                    else:
                        all_vars[arg] = Variable(arg)
                        args[idx] = all_vars[arg]
        super().__init__(*args, **kwargs)
        # variables with same name must be the same variable instance
        names = list(
            {object.__hash__(var): var.name for var in search_variables(self)}.values())
        if len(set(names)) != len(names):
            bad_vars = [item[0] for item in filter(
                lambda x: x[1] > 1, Counter(names).items())]
            raise ValueError(
                f'duplicate variable names detected: {" ".join(bad_vars)}')

    @property
    def unbound_variables(self):
        return {var.name: var for var in search_variables(self)}

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
