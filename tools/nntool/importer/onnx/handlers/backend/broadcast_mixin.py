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
from graph.dim import Dim
from graph.types.input_output import ConstantInputParameters
from importer.common.provisional_dim import ProvisionalDim


class BroadcastMixin(object):

    @classmethod
    def get_broadcasted_shape(cls, x, y):
        if len(x) < len(y):
            x = ([1] * (len(y) - len(x))) + x
        elif len(y) < len(x):
            y = ([1] * (len(x) - len(y))) + y

        assert all(elem_x == elem_y or (elem_x == 1 or elem_y == 1) for elem_x, elem_y in zip(x, y)
                   if elem_x is not None and elem_y is not None),\
            "{} and {} cannot be broadcasted".format(",".join(x), ",".join(y))

        def broad(elem_x, elem_y):
            if elem_x is None or elem_y is None:
                return None
            return elem_x if elem_y == 1 else elem_y
        return [broad(elem_x, elem_y) for elem_x, elem_y in zip(x, y)]

    @classmethod
    def _remove_none_dims_on_constants(cls, inputs):
        const_inputs = set([inp[0]
                            for inp in inputs if isinstance(inp[0], ConstantInputParameters)])
        if not const_inputs:
            return
        non_const_shapes = [inp[2].shape for inp in inputs if inp[0] not in const_inputs]
        max_len = max(len(shape) for shape in non_const_shapes)
        non_const_shapes = [[1] * (max_len - len(shape)) + shape for shape in non_const_shapes]
        none_axes = [any(dim is None for dim in dims) for dims in zip(*non_const_shapes)]
        for inp in const_inputs:
            start = max_len - len(inp.value.shape)
            del_axes = tuple([idx for idx in range(len(inp.value.shape)) if none_axes[start + idx]])
            if not del_axes:
                continue
            inp.value = np.squeeze(inp.value, axis=del_axes)
            inp.dims = Dim.unnamed(inp.value.shape)

    @classmethod
    def _fix_constant_inputs(cls, inputs, shape):
        none_axes = tuple([idx for idx, dim in enumerate(shape) if dim is None])
        const_inputs = set([inp[0]
                            for inp in inputs if isinstance(inp[0], ConstantInputParameters)])
        if not const_inputs:
            return
        for inp in const_inputs:
            inp.value = np.reshape(
                inp.value, [1] * (len(shape) - len(inp.value.shape)) + list(inp.value.shape))
            if none_axes:
                inp.value = np.squeeze(inp.value, axis=none_axes)
            inp.dims = Dim.unnamed(inp.value.shape)

    @classmethod
    def get_broadcast_to(cls, in_shape, shape):
        len_in_shape, len_shape = len(in_shape), len(shape)
        if len_in_shape < len_shape:
            in_shape = [1] * (len_shape - len_in_shape) + in_shape
        elif len_shape < len_in_shape:
            if not all(dim == 1 for dim in in_shape[:(len_in_shape-len_shape):]):
                ValueError("cannot broadcast value of shape %s to shape %s" % (in_shape, shape))
            in_shape = in_shape[(len_in_shape - len_shape)::]
        if not all(in_dim == 1 or in_dim == shape[idx] for idx, in_dim in enumerate(in_shape)):
            ValueError("cannot broadcast value of shape %s to shape %s" % (in_shape, shape))
        return in_shape

    @classmethod
    def implied_broadcast(cls, inputs):
        x_shape = inputs[0][2].shape
        y_shape = inputs[1][2].shape
        new_shape = cls.get_broadcasted_shape(x_shape, y_shape)
        cls._fix_constant_inputs(inputs, new_shape)
        return [ProvisionalDim(new_shape)]

    @classmethod
    def broadcast_to(cls, inp, shape):
        x_shape = inp[2].shape
        new_shape = cls.get_broadcast_to(x_shape, shape)
        return ProvisionalDim(new_shape)
