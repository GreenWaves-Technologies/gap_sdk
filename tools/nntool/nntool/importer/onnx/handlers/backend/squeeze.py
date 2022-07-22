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

from nntool.graph.types import ConstantInputNode, NNEdge, ReshapeNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("Squeeze")
class Squeeze(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        axes = cls._resolve_negative_ranks(kwargs['axes'], len(x_shape))
        if axes:
            if any(x_shape[axis] != 1 and x_shape[axis] is not None for axis in axes):
                raise ValueError("axis parameter in node %s is invalid %s" % (valid_name, axes))
            new_shape = [dim for idx, dim in enumerate(x_shape) if idx not in axes]
        else:
            new_shape = [dim for dim in x_shape if dim != 1]

        pshape = ProvisionalDim(new_shape)
        if cls.is_constant(x):
            logger.info("reducing %s to a constant", valid_name)
            x_val = cls.get_constant(x)
            params = ConstantInputNode(valid_name, value=x_val.reshape(new_shape))
        else:
            old_shape = cls._get_real_dim(x_shape)
            shape = cls._get_real_dim(new_shape)
            params = ReshapeNode(valid_name, old_shape=old_shape, shape=shape)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pshape, x[3])
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        axes = node.attrs.get('axes', tuple())
        return cls._common(node, axes=axes, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        axes = node.attrs.get('axes', tuple())
        return cls._common(node, axes=axes, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        if node.input[1]:
            y = kwargs['all_nodes'][node.input[1]]
            axes = tuple(cls.get_constant(y))
        else:
            axes = tuple()
        return cls._common(node, axes=axes, **kwargs)
