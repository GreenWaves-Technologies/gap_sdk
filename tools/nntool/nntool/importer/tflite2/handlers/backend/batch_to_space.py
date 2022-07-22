# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from nntool.graph.types import (BatchToSpaceNode, ConstantInputNode, NNEdge,
                                ReshapeNode)
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import LOG, TFLiteImportException
from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.utils.batch_to_space import (batch_to_space_nd,
                                         batch_to_space_shape_nd)

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("BATCH_TO_SPACE_ND")
class BatchToSpace(BackendHandler, ConstantMixin):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]

        x = inputs[0]
        x_shape = list(x[2].shape)

        blocks = list(cls._verify_constant(inputs[1]).astype(np.int32).flatten())
        crops = list(list(elem) for elem in cls._verify_constant(inputs[2]).astype(np.int32))

        if any(dim is None for dim in x_shape[1:]):
            raise TFLiteImportException(
                f'BATCH_TO_SPACE_ND {node.name} only batch dim can be unknown')

        if cls._is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            value = batch_to_space_nd(cls._get_constant(x), blocks, crops)
            out_shape = value.shape
            params = ConstantInputNode(node.name, value=value)
        else:
            if x_shape[0] is None:
                shape = [1] + x_shape[1:]
                reshape = ReshapeNode(
                    G.unique_name(f'{node.name}_batch'),
                    old_shape=x_shape,
                    shape=shape
                )
                x_shape = shape
                G.add_edge(
                    NNEdge(from_node=x[0], from_idx=x[1], to_node=reshape))
                x = (reshape, 0)

            params = BatchToSpaceNode(node.name, block_shape=blocks, crops=crops)
            out_shape = batch_to_space_shape_nd(x_shape, blocks, crops)

        pout_shape = ProvisionalDim(out_shape)
        if opts.get('load_quantization'):
            G.quantization[params.name] = cls.load_tf_quantization(
                [node.input[0]], node.output)
        G.add_edge(NNEdge(from_node=x[0], to_node=params,
                          from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pout_shape)
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
