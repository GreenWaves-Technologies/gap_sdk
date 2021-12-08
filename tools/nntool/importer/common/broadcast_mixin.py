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
from graph.types import ConstantInputParameters

from .provisional_dim import ProvisionalDim


class BroadcastMixin(object):

    @classmethod
    def get_broadcasted_shape(cls, x, y):
        if len(x) < len(y):
            x = ([1] * (len(y) - len(x))) + x
        elif len(y) < len(x):
            y = ([1] * (len(x) - len(y))) + y

        assert all(elem_x == elem_y or (elem_x == 1 or elem_y == 1) for elem_x, elem_y in zip(x, y)
                   if elem_x is not None and elem_y is not None),\
            "{} and {} cannot be broadcasted".format(x, y)

        def broad(elem_x, elem_y):
            if elem_x is None or elem_y is None:
                return None
            return elem_x if elem_y == 1 else elem_y
        return [broad(elem_x, elem_y) for elem_x, elem_y in zip(x, y)]

    @classmethod
    def _fix_constant_inputs(cls, inputs, shape):
        none_axes = tuple([idx for idx, dim in enumerate(shape) if dim is None])
        const_inputs = list([inp
                             for inp in inputs if isinstance(inp[0], ConstantInputParameters)])
        if not const_inputs:
            return
        for inp in const_inputs:
            node = inp[0]
            node.value = np.reshape(
                node.value, [1] * (len(shape) - len(node.value.shape)) + list(node.value.shape))
            if none_axes:
                node.value = np.squeeze(node.value, axis=none_axes)
            # setting the provisional shape here is a little dangerous
            # if the unknown axis is first then it works but if it is
            # in the middle and this value is connected to another node then
            # could be problematic (but it is problematic anyway since it won't
            # expect something broadcasted)
            inp[2].shape = list(node.value.shape)
            node.dims = Dim.unnamed(node.value.shape)

    @classmethod
    def implied_broadcast(cls, inputs):
        x = inputs[0][2].shape
        y = inputs[1][2].shape
        shape = cls.get_broadcasted_shape(x, y)
        cls._fix_constant_inputs(inputs, shape)
        return [ProvisionalDim(shape)]
