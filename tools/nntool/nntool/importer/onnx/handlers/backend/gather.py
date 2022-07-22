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
from nntool.graph.types import ConstantInputNode, GatherNode, NNEdge, ReshapeNode, StridedSliceNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger
from nntool.utils.numpy_helpers import np_asscalar

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("Gather")
class Gather(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        y = inputs[1]
        y_shape = y[2].shape
        indices = cls.get_constant(y)
        axis = node.attrs.get('axis', 0)

        if not y_shape:
            pshape = ProvisionalDim(
                x_shape[:axis:] + x_shape[axis + 1:])
        else:
            pshape = ProvisionalDim(
                x_shape[:axis:] + list(indices.shape) + x_shape[axis + 1:])
        if cls.is_constant(x):
            x_val = cls.get_constant(x)
            logger.info(
                f"reducing {valid_name} to a constant {cls.print_small(x_val)}")
            params = ConstantInputNode(valid_name, value=np.take(
                x_val, indices.astype(np.int64), axis=axis))
        else:
            if np.ndim(indices) <= 1:
                idx = np_asscalar(indices)
                act_slice = tuple([(0, dim, 1) if i != axis else (
                    idx, idx+1, 1) for i, dim in enumerate(x_shape) if dim is not None])
                out_shape = pshape.known_shape.copy()
                params = StridedSliceNode(
                    valid_name, act_slice=act_slice, out_shape=out_shape)
                if params.slice_shape == tuple(x[2].known_shape):
                    if np.ndim(indices) == 0 and pshape.shape[idx] is not None:
                        del out_shape[idx]
                        pshape = ProvisionalDim(out_shape)
                    params = ReshapeNode(valid_name, old_shape=tuple(
                        x[2].known_shape), shape=out_shape)
            else:
                axis = cls._trim_axis(axis, x_shape)
                params = GatherNode(
                    valid_name, axis=axis, indices=indices)
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pshape, x[3])
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
