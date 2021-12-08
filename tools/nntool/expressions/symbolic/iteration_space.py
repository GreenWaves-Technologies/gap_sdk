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

from functools import partial, reduce
from itertools import groupby
from operator import itemgetter
from typing import List, Sequence

import numpy as np
from quantization.qtype import DTYPE_GAP_CTYPE

from .symbol import Symbol
from .variable_container import VariableContainerAndAssigner


def split_list(sequence: Sequence):
    return [list(map(itemgetter(1), g)) for k, g in groupby(enumerate(sequence), lambda x: x[0]-x[1])]


class IterationSpace():
    def __init__(self, assigner: VariableContainerAndAssigner, min_interior_space=1000) -> None:
        self._assigner = assigner
        self._var_shapes = None
        self._var_axes = None
        self._spaces = None
        self._min_interior_space = min_interior_space
        self._num_workers = 8
        self._var_strides = {}

    @staticmethod
    def _var_name(idx):
        return f"i{idx}"

    @staticmethod
    def _var_max_name(idx):
        return f"I{idx}"

    @staticmethod
    def _par_name(idx):
        return f"d{idx}"

    @staticmethod
    def _par_first_name(idx):
        return f"D{idx}First"

    @staticmethod
    def _par_last_name(idx):
        return f"D{idx}Last"

    @staticmethod
    def _par_max_name(idx):
        return f"D{idx}"

    def set_var_stride(self, vname, stride):
        self._var_shapes = None
        self._var_axes = None
        self._spaces = None
        self._var_strides[vname] = stride

    @property
    def shape(self):
        return max(zip(*Symbol.extend_shapes(*self._assigner.unbound_shapes)))

    @property
    def full_rank(self):
        return len(self.shape)

    @property
    def extended_strides(self):
        full_len = self.full_rank
        if not self._var_strides:
            return [frozenset([1])] * full_len
        vstrides = self._var_strides.copy()
        vshapes = self.var_shapes
        for vname in self.variables:
            if vname not in vstrides:
                vstrides[vname] = [{} if dim == 1 else {1}
                                   for dim in vshapes[vname]]
            else:
                vstrides[vname] = [{dim} if vshapes[vname][idx] != 1 else {
                } for idx, dim in enumerate(vstrides[vname])]
            vstrides[vname] = (
                [{}] * (full_len - len(vstrides[vname]))) + list(vstrides[vname])
        return tuple(reduce(lambda state, x: state + [frozenset(set.union(*x))], zip(*tuple(vstrides.values())), []))

    @property
    def produced_variables(self):
        return set(self._assigner.returned_variables)

    @property
    def consumed_variables(self):
        return set(self._assigner.unbound_variables)

    @property
    def temporary_variables(self):
        return set(self.variables) - self.produced_variables - self.consumed_variables

    def space_for_axis(self, axis):
        return next((axes for axes in self.spaces if axis in axes), None)

    @property
    def var_axes(self):
        """Map of variable name to tuple of axes of iteration space used

        Returns:
            dict: Map of variable name to tuple of axes of iteration space used
        """
        if self._var_axes is None:
            self._var_axes = {vname: tuple(sorted(set(self.space_for_axis(axis)
                                                      for axis, dim in enumerate(shape) if dim != 1)))
                              for vname, shape in self.var_shapes.items()}
        return self._var_axes

    @property
    def variables(self):
        """Set of variable names
        """
        return set(self.var_shapes)

    @property
    def spaces(self):
        """Set of disjoint iteration spaces that have the same set of strides
        """
        if self._spaces is None:
            spaces = self._assigner.axes
            # here we modify grouped spaces so that continuous spaces have the same stride
            if self._var_strides:
                final_spaces = list(spaces)
                for var_stride in self._var_strides.values():
                    # expand the stride to the full shape
                    # The [1] may be wrong when we expand to supporting non unit strides
                    # perhaps set to None and then remove and add back to the largest space in the reduction?
                    var_stride = (
                        [1] * (self.full_rank - len(var_stride))) + list(var_stride)

                    def reduction(var_stride, state: List, space):
                        space_strides = {}
                        for dim in space:
                            space_strides.setdefault(var_stride[dim], []).append(dim)
                        for space_group in space_strides.values():
                            state.extend(split_list(space_group))
                        return state

                    final_spaces = reduce(partial(reduction, var_stride), final_spaces, [])
                self._spaces = tuple(sorted(tuple(sorted(space))
                                            for space in final_spaces))
            else:
                self._spaces = spaces

        return self._spaces

    @property
    def var_shapes(self):
        if self._var_shapes is None:
            self._var_shapes = self._assigner.var_shapes.copy()
            self._var_shapes = dict(
                zip(self._var_shapes.keys(), Symbol.extend_shapes(*self._var_shapes.values())))

        return self._var_shapes

    @property
    def axis_shape(self):
        return tuple(np.prod([self.shape[axis] for axis in axis_set]) for axis_set in self.spaces)

    @property
    def iterator_names(self):
        return [self._var_name(idx) for idx in range(len(self.spaces))]

    @property
    def interior_space(self):
        """This provides the minimum tile space if it is more than one axis"""
        dims = []
        shape = list(self.axis_shape)
        total = 1
        while len(shape) > 1 and total < self._min_interior_space:
            dims.append(len(shape) - 1)
            total *= shape[-1]
            shape = shape[0:-1]
        return tuple(reversed(dims))

    @property
    def interior_shape(self):
        shape = list(self.axis_shape)
        return tuple(shape[idx] for idx in self.interior_space)

    @property
    def exterior_spaces(self):
        return tuple(range(len(self.axis_shape) - len(self.interior_space)))

    @property
    def exterior_space_names(self):
        return tuple(self._par_name(idx) for idx in range(len(self.exterior_spaces)))

    @property
    def exterior_shape(self):
        shape = list(self.axis_shape)
        num_ext_spaces = len(self.exterior_spaces)
        return tuple(shape[:num_ext_spaces - 1] + [shape[num_ext_spaces - 1] * np.prod(self.interior_shape)])

    @property
    def parameteric_spaces(self):
        return tuple(self.exterior_spaces[-2:])

    @property
    def interior_shape_size(self):
        return np.prod(self.interior_shape)

    @property
    def fixed_spaces(self):
        return tuple(self.exterior_spaces[:-2])

    def preferred_divisor(self, space):
        if space == 0:
            if len(self.exterior_shape) == 1:
                return self.interior_shape_size * self._num_workers
            else:
                return self._num_workers
        elif space == len(self.exterior_shape) - 1:
            return self.interior_shape_size
        return 1

    def c_indexed_var(self, var_name, declare=False, assignment=False):
        if var_name in self.temporary_variables:
            if declare or assignment:
                dtype = self._assigner.find(var_name).dtype
                return f"{DTYPE_GAP_CTYPE[dtype]} {var_name}"
            return var_name
        if declare:
            dtype = self._assigner.find(var_name).dtype
            return f"{DTYPE_GAP_CTYPE[dtype]} *{var_name}"
        return f'{var_name}{self.c_index(var_name)}'

    def c_index(self, var_name):
        var_spaces = [self.spaces.index(space)
                      for space in self.var_axes[var_name]]
        var_ext_shape = self.var_shapes[var_name]
        var_shape = [np.prod([var_ext_shape[dim] for dim in space])
                     for space in self.var_axes[var_name]]
        if var_name in self._var_strides:
            var_stride = self._var_strides[var_name]
            var_stride_in_space = []
            for space in self.var_axes[var_name]:
                var_stride_in_space.append(var_stride[space[0]])
                assert all(var_stride_in_space[-1] ==
                           var_stride[dim] for dim in space[1:])
        else:
            var_stride_in_space = [1] * len(var_spaces)

        def reduction(state, x):
            var_space, space_dim, var_stride = x
            # fixed spaces are iterated by tiler code
            if var_space in self.fixed_spaces:
                return state
            space_size = str(
                space_dim) if var_space in self.interior_space else self._var_max_name(var_space)
            assert abs(var_stride) == 1, "non unit strides not supported yet"
            if var_stride < 0:
                if var_space == 0:
                    index = f'(Last-1-{self._var_name(var_space)})'
                else:
                    index = f'({space_size}-1-{self._var_name(var_space)})'
            else:
                index = f'{self._var_name(var_space)}'
            if state[0]:
                state[1].insert(
                    0, f"({index}*{'*'.join(state[0])})")
            else:
                state[1].insert(0, index)

            state[0].insert(0, str(
                space_dim) if var_space in self.interior_space else self._var_max_name(var_space))
            return state
        index = reduce(reduction, zip(reversed(var_spaces),
                                      reversed(var_shape),
                                      reversed(var_stride_in_space)), ([], []))[1]
        return f"[{'+'.join(index)}]"

    def c_for(self, space):
        if space in self.fixed_spaces:
            raise ValueError(
                "space is fixed so not iterated and requires no for loop")
        var_name = self._var_name(space)
        if space in self.interior_space:
            return f"for (int {var_name}=0; {var_name}<{self.shape[space]}; {var_name}++)"
        if space == 0:
            return f"for (int {var_name}=First; {var_name}<Last; {var_name}++)"
        return f"for (int {var_name}=0; {var_name}<{self._var_max_name(space)}; {var_name}++)"
