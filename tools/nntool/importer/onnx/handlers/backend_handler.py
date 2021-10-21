
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
from graph.types import (ConstantInputParameters, ReshapeParameters,
                         TransposeParameters)
from graph.types.input_output import InputParameters

from .handler import Handler


class BackendHandler(Handler):
    """ This class is base backend handler class.
    All backend operator handler class MUST inherit this class.
    In backend, operator handler class's name should be pascal case of file name
    which should be snake case.
    Use ONNX operator name as class name.
    """

    VAR_COUNT = 0

    @classmethod
    def _get_real_dim(cls, shape):
        return list(elem for elem in shape if elem is not None)

    @classmethod
    def _get_real_dims(cls, dims):
        return [cls._get_real_dim(dim.shape) for dim in dims]

    @classmethod
    def _slice_len(cls, vstart, vend, vstep):
        if vstep < 0:
            vstart, vend = vend, vstart
            vstep = -vstep
        return (vend - vstart - 1) // vstep + 1

    @classmethod
    def _resolve_negative_ranks(cls, shape, rank):
        return [elem if elem >= 0 else rank + elem for elem in shape]

    @classmethod
    def _trim_axis(cls, axis, shape):
        return axis - sum([1 if dim is None else 0 for dim in shape[:axis:]])

    @classmethod
    def print_small(cls, tensor):
        if tensor.size < 6:
            return str(tensor)
        return ""
