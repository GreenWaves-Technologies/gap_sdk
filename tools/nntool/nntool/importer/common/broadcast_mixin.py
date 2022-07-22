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
from nntool.graph.dim import Dim
from nntool.graph.types import ConstantInputNode

from .provisional_dim import ProvisionalDim

# reduces broadcasted constants on unknown dimensions.
# Setting this to false can provoke conception errors in matchers
FIX_CONSTANTS = True

class BroadcastMixin(object):

    @classmethod
    def get_broadcasted_shape(cls, x, y, is_constant=None):
        if is_constant is None:
            is_constant = (False, False)
        if len(x) < len(y):
            x = ([1] * (len(y) - len(x))) + x
        elif len(y) < len(x):
            y = ([1] * (len(x) - len(y))) + y

        assert all(elem_x == elem_y or (elem_x == 1 or elem_y == 1) for elem_x, elem_y in zip(x, y)
                   if elem_x is not None and elem_y is not None),\
            "{} and {} cannot be broadcasted".format(x, y)

        def broad(elem_x, elem_y):
            # if one element is not None then take it since that dimension will be broadcasted
            if elem_x is None:
                if elem_y is None or (FIX_CONSTANTS and is_constant[1] and elem_y == 1):
                    return None
                else:
                    return elem_y
            else:
                if elem_y is None:
                    if FIX_CONSTANTS and is_constant[0] and elem_x == 1:
                        return None
                    else:
                        return elem_x
                else:
                    return elem_x if elem_y == 1 else elem_y
        return [broad(elem_x, elem_y) for elem_x, elem_y in zip(x, y)]

    @classmethod
    def _fix_constant_inputs(cls, inputs, shape):
        const_inputs = list([inp
                             for inp in inputs if isinstance(inp[0], ConstantInputNode)])
        if not const_inputs or not shape:
            return
        for inp in const_inputs:
            node = inp[0]
            cur_shape = tuple(node.value.shape)
            new_shape = []
            for i in range(1, len(cur_shape) + 1):
                if shape[-i] is None:
                    if cur_shape[-i] != 1:
                        raise ValueError('unknown dimension in constant is not equal to 1')
                else:
                    new_shape.insert(0, cur_shape[-i])
            new_shape = tuple(new_shape)
            if new_shape != cur_shape:
                node.value = np.reshape(node.value, new_shape)
                inp[2].shape = list(new_shape)
                node.dims = Dim.unnamed(new_shape)

    @classmethod
    def implied_broadcast(cls, inputs):
        is_constant = [isinstance(inp[0], ConstantInputNode) for inp in inputs]
        x = inputs[0][2].shape
        y = inputs[1][2].shape
        shape = cls.get_broadcasted_shape(x, y, is_constant=is_constant)
        cls._fix_constant_inputs(inputs, shape)
        return [ProvisionalDim(shape)]
