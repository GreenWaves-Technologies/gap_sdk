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
from numpy.core.fromnumeric import prod
from graph.dim import Dim
from graph.types import ConstantInputParameters, NNEdge, ReshapeParameters
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from importer.common.constant_mixin import ConstantMixin


@onnx_op("Reshape")
class Reshape(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]

        if cls.SINCE_VERSION == 1:
            shape = np.array(node.attrs["shape"])
        else:  # since_version >= 5
            shape = cls.get_constant(inputs[1])

        if cls.is_constant(inputs[0]):
            logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputParameters(
                valid_name,
                value=cls.get_constant(inputs[0]).reshape(shape)
            )
            pshape = ProvisionalDim(shape)
            all_nodes[node.output[0]] = (params, 0, pshape)
            return params

        input_shape = np.array(inputs[0][2].shape)
        shape = [dim if dim != 0 else input_shape[idx] for idx, dim in enumerate(shape)]
        if -1 in shape:
            wild_index = shape.index(-1)
            in_size = prod([1 if dim is None else dim for dim in input_shape])
            shape_size = prod([1 if dim is None or dim <= 0 else dim for dim in shape])
            if in_size % shape_size != 0:
                raise ValueError('invalid reshape')
            shape[wild_index] = in_size // shape_size
        shape = np.array(shape)

        # TODO - There must be a better way of doing this
        # This hacks around the fact that the batch dimension will be in the reshape
        if input_shape[0] is None and shape[0] == 1:
            shape = np.array([None]+list(shape[1::]))

        pshape = ProvisionalDim(shape)
# pylint: disable=singleton-comparison
        old_shape = Dim.unnamed(list(input_shape[input_shape != None]))
        shape = Dim.unnamed(list(shape[shape != None]))
        params = ReshapeParameters(valid_name, old_shape=old_shape, shape=shape)
        inp = inputs[0]
        G.add_edge(NNEdge(from_node=inp[0], to_node=params, from_idx=inp[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pshape)
        return params


    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_5(cls, node, **kwargs):
        return cls._common(node, **kwargs)
