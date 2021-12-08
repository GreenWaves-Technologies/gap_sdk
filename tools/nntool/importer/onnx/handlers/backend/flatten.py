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

from graph.dim import Dim
from graph.types import ConstantInputParameters, NNEdge, ReshapeParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op


def prod(iterable):
    return reduce(operator.mul, iterable, 1)

@onnx_op("Flatten")
class Flatten(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        axis = node.attrs.get('axis', 1)
        if axis < 0:
            axis += len(x_shape)

        old_shape = cls._get_real_dim(x_shape)
        if axis == 0:
            shape = [1, prod(old_shape)]
            pshape = shape
        else:
            start = x_shape[:axis:]
            end = x_shape[axis::]
            pshape = list(start) + [prod(cls._get_real_dim(end))]
            shape = cls._get_real_dim(pshape)

        if cls.is_constant(x):
            logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputParameters(
                valid_name,
                value=cls.get_constant(x).reshape(shape)
            )
            pshape = ProvisionalDim(shape)
        else:
            params = ReshapeParameters(valid_name, old_shape=Dim.unnamed(
                old_shape), shape=Dim.unnamed(shape))
            pshape = ProvisionalDim(pshape)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        all_nodes[node.output[0]] = (params, 0, pshape, x[3])
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_9(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
