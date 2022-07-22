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
from nntool.graph.types import NNEdge, ReshapeNode
from nntool.graph.types.constant_input import ConstantInputNode
from nntool.graph.types.resizers import (BilinearResizerNode,
                                  NearestNeighborResizerNode)
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger
from pytest import param

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("Upsample")
class Upsample(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, inputs, scales, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        x = inputs[0]
        x_shape = x[2].shape
        x_rank = len(x_shape)

        mode = node.attrs.get('mode', 'nearest')

        spatial_size = x_rank - 2
        in_c = x_shape[1]
        in_w = x_shape[-1]
        sizes = [int(shape * scale) if shape is not None else None
                    for shape, scale in zip(x_shape, scales)]
        
        if np.prod([sz for sz in sizes if sz is not None]) == 0:
            logger.warn(f'{valid_name} has null output shape')
            params = ConstantInputNode(valid_name, value=np.array([]))
            all_nodes[node.output[0]] = (params, 0, ProvisionalDim([]), x[3])
            return params

        if spatial_size == 1:
            sizes.insert(-1, 1)

        if spatial_size != 2 and spatial_size != 1:
            raise ValueError('resize only supports 4D tensor in NCHW mode or 3D tensor in NCF mode'
                             f' - input shape is {x_shape} sizes is {sizes}')

        if not all(x_dim == size_dim for x_dim, size_dim in zip(x_shape[:2:], sizes[:2:])):
            raise ValueError('resize only supports 4D tensor in NCHW mode or 3D tensor in NCF mode'
                             f' - input shape is {x_shape} sizes is {sizes}')

        params_class = BilinearResizerNode if mode == 'linear' else NearestNeighborResizerNode

        params = params_class(valid_name,
                              new_shape=tuple(sizes[2::]),
                              align_corners=False,
                              halfpixel_centers=False,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])

        if spatial_size == 1:
            r1_params = ReshapeNode(f'{valid_name}_reshape2d',
                                          old_shape=Dim.unnamed([in_c, in_w]),
                                          shape=Dim.unnamed([in_c, 1, in_w]))
            r2_params = ReshapeNode(f'{valid_name}_reshape1d',
                                          old_shape=Dim.unnamed(
                                              [in_c, 1, sizes[-1]]),
                                          shape=Dim.unnamed([in_c, sizes[-1]]))
            G.add_edge(
                NNEdge(from_node=x[0], to_node=r1_params, from_idx=x[1], to_idx=0))
            G.add_edge(NNEdge(from_node=r1_params,
                              to_node=params, from_idx=0, to_idx=0))
            G.add_edge(NNEdge(from_node=params,
                              to_node=r2_params, from_idx=0, to_idx=0))
            pout_dims = ProvisionalDim(sizes[:-2:] + sizes[-1::])
            params = r2_params
        else:
            pout_dims = ProvisionalDim(sizes)
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        all_nodes[node.output[0]] = (params, 0, pout_dims, x[3])
        return params

    @classmethod
    def version_7(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[inp] if inp else None for inp in node.input]
        scales = node.attrs['scales']
        return cls._common(node, inputs, scales, **kwargs)

    @classmethod
    def version_9(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[inp] if inp else None for inp in node.input]
        scales = cls.get_constant(inputs[1])
        return cls._common(node, inputs, scales, **kwargs)

    @classmethod
    def version_10(cls, node, **kwargs):
        return cls.version_9(node, **kwargs)
