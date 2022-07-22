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
from nntool.graph.types.activations import SoftMaxNode
from nntool.graph.types.base import NNEdge
from nntool.graph.types import ReshapeNode
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.handlers.backend.math_mixin import BasicMathMixin

from ..backend_handler import BackendHandler
from ..handler import onnx_op


def condense(shape):
    if all(dim is None for dim in shape):
        return None
    return int(np.prod([dim for dim in shape if dim is not None]))


@onnx_op("Softmax")
class Softmax(BasicMathMixin, BackendHandler):

    @classmethod
    def _common1_11(cls, node, **kwargs):
        axis = node.attrs.get('axis', 1)
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        x = all_nodes[node.input[0]]
        x_shape = x[2].shape
        if axis < 0:
            axis += len(x_shape)
        old_shape = cls._get_real_dim(x_shape)
        # v 1 and 11 work differently to v13. In v1 and v11 the input is collected into a 2D tensor
        # based on the axis [a_0, a_1, ..., a_{k-1}, a_k, ..., a_{n-1}]  with axis k
        # becomes [a_0 * ... * a_{k-1}, a_k * ... * a_{n-1}]
        # This is used for the softmax
        new_pshape = [condense(x_shape[:axis:]),
                      condense(x_shape[axis::])]
        new_shape = cls._get_real_dim(new_pshape)
        reshape_1 = ReshapeNode(valid_name + "_reshape1",
                                      old_shape=old_shape, shape=new_shape)
        G.add_edge(NNEdge(from_node=x[0], to_node=reshape_1, from_idx=x[1], to_idx=0))
        # operation axis will either be 1 or 0
        softmax = SoftMaxNode(valid_name, axis=len(new_shape) - 1)
        G.add_edge(NNEdge(from_node=reshape_1, to_node=softmax))
        reshape_2 = ReshapeNode(valid_name + "_reshape2",
                                      old_shape=new_shape, shape=old_shape)
        G.add_edge(NNEdge(from_node=softmax, to_node=reshape_2))

        all_nodes[node.output[0]] = (reshape_2, 0, ProvisionalDim(x_shape), None)
        return softmax

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common1_11(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common1_11(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        axis = node.attrs.get('axis', -1)
        all_nodes = kwargs['all_nodes']
        x = all_nodes[node.input[0]]
        x_shape = x[2].shape
        if axis < 0:
            axis += len(x_shape)
        axis = cls._trim_axis(axis, x_shape)
        return super(Softmax, cls)._common(node, params_class=SoftMaxNode,
                                           params_args={'axis': axis}, **kwargs)
