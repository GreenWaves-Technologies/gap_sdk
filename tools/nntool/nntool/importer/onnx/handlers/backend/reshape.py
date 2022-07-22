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

from typing import Counter
import numpy as np
from numpy.core.fromnumeric import prod
from functools import reduce
from nntool.graph.dim import Dim
from nntool.graph.types import ConstantInputNode, NNEdge, ReshapeNode
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from nntool.importer.common.constant_mixin import ConstantMixin


@onnx_op("Reshape")
class Reshape(ConstantMixin, BackendHandler):
    @classmethod
    def moves_unknown(cls, inp, shape):
        if None not in inp or Counter(inp)[None] > 1:
            return False
        if -1 not in shape or None in shape:
            return False

        inp_wo_nones = [dim for dim in inp if dim != None]
        shape_wo_all = [dim for dim in shape if dim != -1]
        if np.prod(inp_wo_nones) != np.prod(shape_wo_all):
            return False
        none_idx = list(inp).index(None)
        return none_idx >= len(shape_wo_all) or shape_wo_all[none_idx] != -1


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

        input_shape = np.array(inputs[0][2].shape)
        shape = [dim if dim != 0 else input_shape[idx] for idx, dim in enumerate(shape)]
        # this catches a special case where inp is something like [None, 2, 4] and shape is [2, -1, 4]
        # The -1 is clearly the None moving so move it
        if cls.moves_unknown(input_shape, shape):
            shape = np.array([None if dim == -1 else dim for dim in shape])
        else:
            if -1 in shape:
                new_shape_size = reduce(lambda x, y: x * 1 if y is None or y == -1 else x * y, shape, 1)
                inp_size = reduce(lambda x, y: x * y if y is not None else x, input_shape, 1)
                in_size = prod([1 if dim is None else dim for dim in input_shape])
                shape_size = prod([1 if dim is None else dim for dim in shape])
                if in_size % shape_size != 0:
                    raise ValueError('invalid reshape')
                shape[shape.index(-1)] = inp_size // new_shape_size
            shape = np.array(shape)
            # TODO - There must be a better way of doing this
            # This hacks around the fact that the batch dimension will be in the reshape
            if input_shape[0] is None and shape[0] == 1:
                shape = np.array([None]+list(shape[1::]))

        inp = inputs[0]
        if cls.is_constant(inp):
            # there should be no None in shape since a constant always has known size
            logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputNode(
                valid_name,
                value=cls.get_constant(inp).reshape(shape),
                dims=Dim.unnamed(shape)
            )
            pshape = ProvisionalDim(shape)
            all_nodes[node.output[0]] = (params, 0, pshape, inp[3])
            return params

        pshape = ProvisionalDim(shape)
# pylint: disable=singleton-comparison
        old_shape = Dim.unnamed(list(input_shape[input_shape != None]))
        shape = Dim.unnamed(list(shape[shape != None]))
        params = ReshapeNode(valid_name, old_shape=old_shape, shape=shape)
        G.add_edge(NNEdge(from_node=inp[0], to_node=params, from_idx=inp[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pshape, inp[3])
        return params


    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_5(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_14(cls, node, **kwargs):
        return cls._common(node, **kwargs)
