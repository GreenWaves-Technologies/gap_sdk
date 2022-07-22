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
from nntool.graph.types import NNEdge
from nntool.graph.types.constant_input import ConstantInputNode
from nntool.graph.types.tensor_shape import RepeatNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import LOG
from nntool.importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("TILE")
class Tile(BackendHandler, ConstantMixin):

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        repeats = list(cls._verify_constant(inputs[1]).flatten())
        node.input[1].used = True
        x = inputs[0]
        x_shape = x[2].shape
        if np.prod(repeats) != np.max(repeats):
            raise NotImplementedError(f"Only one axis repeat supported: layer {node.name} has {repeats} repeat pattern")

        new_shape = [s * r if s is not None else None for s, r in zip(x_shape, repeats)]
        for i, s in enumerate(x_shape):
            if s is None:
                del repeats[i]
        repeat_axis = np.argmax(repeats)
        n_repeats = np.max(repeats)

        pnew_shape = ProvisionalDim(new_shape)
        if cls.is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            val = np.repeat(cls.get_constant(x), n_repeats, axis=repeat_axis)
            params = ConstantInputNode(node.name, value=val, dims=Dim.unnamed(val.shape))
        else:
            params = RepeatNode(
                node.name, n_repeats=n_repeats, repeat_axis=repeat_axis)

        if opts.get('load_quantization'):
            G.quantization[params.name] = cls.load_tf_quantization(
                [node.input[0]], node.output)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pnew_shape)
        return params

# Martin alternative implementation
# @tflite_op("TILE")
# class Tile(BackendHandler, ConstantMixin):

#     @classmethod
#     def _common(cls, node: TFLiteNode, **kwargs):
#         G = kwargs['G']
#         opts = kwargs['opts']
#         all_nodes = kwargs['all_nodes']

#         inputs = [all_nodes[t] for t in node.input]

#         x = inputs[0]
#         x_shape = list(x[2].shape)
#         reps = list(cls._verify_constant(inputs[1]).astype(np.int32).flatten())
#         if len(reps) != len(x_shape):
#             raise TFLiteImportException(f'{node.name} tile multiples must be same length as input rank')

#         out_shape = [dim * mult if dim is not None else None
#                      for dim, mult in zip(x_shape, reps)]
#         reps = [elem for elem, dim in zip(reps, x_shape) if dim is not None]

#         if cls._is_constant(x):
#             LOG.info("reducing %s to a constant", node.name)
#             value = np.tile(cls._get_constant(x), reps)
#             out_shape = value.shape
#             params = ConstantInputNode(node.name, value=value)
#         else:
#             if np.all(np.array(reps) == 1):
#                 params = NoOPNode(node.name, "tile with 1 repetition")
#             else:
#                 params = TileNode(node.name, reps=reps)

#         pout_shape = ProvisionalDim(out_shape)
#         if opts.get('load_quantization'):
#             G.quantization[params.name] = cls.load_tf_quantization(
#                 [node.input[0]], node.output)
#         G.add_edge(NNEdge(from_node=x[0], to_node=params,
#                           from_idx=x[1], to_idx=0))
#         all_nodes[node.output[0]] = (params, 0, pout_shape)
#         return params

#     @classmethod
#     def version_1(cls, node: TFLiteNode, **kwargs):
#         return cls._common(node, **kwargs)
