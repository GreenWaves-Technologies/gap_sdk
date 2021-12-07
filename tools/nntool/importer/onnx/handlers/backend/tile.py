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

import operator
from functools import reduce

import numpy as np
from graph.dim import Dim
from graph.types import ConstantInputParameters, NNEdge, ReshapeParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op


def prod(iterable):
    return reduce(operator.mul, iterable, 1)

@onnx_op("Tile")
class Tile(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        if not all(cls.is_constant(inp) for inp in inputs):
            raise NotImplementedError("Tile is only implemented on constants")

        inp_vals = [cls.get_constant(inp) for inp in inputs]
        out_val = np.tile(inp_vals[0], inp_vals[1])
        logger.info("reducing %s to a constant", valid_name)
        params = ConstantInputParameters(
            valid_name,
            value=out_val
        )
        pshape = ProvisionalDim(out_val.shape)
        all_nodes[node.output[0]] = (params, 0, pshape, inputs[0][3])
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
