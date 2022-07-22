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
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from nntool.importer.common.constant_mixin import ConstantMixin


@onnx_op("Unsqueeze")
class Unsqueeze(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        out_rank = len(x_shape) + len(kwargs['axes'])
        axes = cls._resolve_negative_ranks(kwargs['axes'], out_rank)

        old_shape = list(x_shape)
        new_shape = [1 if new_idx in axes else old_shape.pop(0)
                     for new_idx in range(out_rank)]

        pshape = ProvisionalDim(new_shape)
        if cls.is_constant(x):
            x_val = cls.get_constant(x)
            logger.info(
                f"reducing {valid_name} to a constant {cls.print_small(x_val)}")
            params = ConstantInputNode(
                valid_name, value=x_val.reshape(new_shape))
        else:
            old_shape = cls._get_real_dim(x_shape)
            shape = cls._get_real_dim(new_shape)
            params = ReshapeNode(
                valid_name, old_shape=old_shape, shape=shape)
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pshape, x[3])
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        axes = node.attrs['axes']
        return cls._common(node, axes=axes, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        axes = node.attrs['axes']
        return cls._common(node, axes=axes, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        y = kwargs['all_nodes'][node.input[1]]
        axes = tuple(cls.get_constant(y))
        return cls._common(node, axes=axes, **kwargs)
