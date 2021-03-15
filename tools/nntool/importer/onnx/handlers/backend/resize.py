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
from graph.dim import Dim
from graph.types import NNEdge, ReshapeParameters
from graph.types.resizers import (BilinearResizerParameters,
                                  NearestNeighborResizerParameters)
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description


@onnx_op("Resize")
@partial_support(True)
@ps_description("""Only 'round_prefer_ceil' is supported as 'nearest_mode'
4D and 3D tensors with input in NCHW and NCF mode with the resize on H and W or F is
supported. The 'coordinate_transformation_mode' is 'half_pixel'.""")
class Resize(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, scales, sizes, nearest_mode='round_prefer_ceil', **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] if inp else None for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        x_rank = len(x_shape)
        spatial_size = x_rank - 2
        in_c = x_shape[1]
        in_w = x_shape[-1]
        if scales is not None:
            sizes = np.array(x_shape) * np.array(scales)
        sizes = [None if x_shape[idx] is None else dim
                 for idx, dim in enumerate(sizes)]
        if spatial_size == 1:
            sizes.insert(-1, 1)

        if nearest_mode != 'round_prefer_ceil':
            logger.warning('only round_prefer_ceil is supported for nearest mode')

        if spatial_size != 2 and spatial_size != 1:
            raise ValueError('resize only supports 4D tensor in NCHW mode or 3D tensor in NCF mode'
                             f' - input shape is {x_shape} sizes is {sizes}')

        if not all(x_dim == size_dim for x_dim, size_dim in zip(x_shape[:2:], sizes[:2:])):
            raise ValueError('resize only supports 4D tensor in NCHW mode or 3D tensor in NCF mode'
                             f' - input shape is {x_shape} sizes is {sizes}')

        mode = node.attrs.get('mode', 'nearest')
        if mode != 'nearest' and mode != 'linear':
            raise ValueError('resize only supports nearest and linear modes')

        params_class = BilinearResizerParameters if mode == 'linear' else NearestNeighborResizerParameters

        params = params_class(valid_name,
                              new_shape=tuple(sizes[2::]),
                              align_corners=False,
                              halfpixel_centers=False,
                              in_dims_hint=[['c', 'h', 'w']],
                              out_dims_hint=[['c', 'h', 'w']])

        if spatial_size == 1:
            r1_params = ReshapeParameters(f'{valid_name}_reshape2d',
                                          old_shape=Dim.unnamed([in_c, in_w]),
                                          shape=Dim.unnamed([in_c, 1, in_w]))
            r2_params = ReshapeParameters(f'{valid_name}_reshape1d',
                                          old_shape=Dim.unnamed([in_c, 1, sizes[-1]]),
                                          shape=Dim.unnamed([in_c, sizes[-1]]))
            G.add_edge(NNEdge(from_node=x[0], to_node=r1_params, from_idx=x[1], to_idx=0))
            G.add_edge(NNEdge(from_node=r1_params, to_node=params, from_idx=0, to_idx=0))
            G.add_edge(NNEdge(from_node=params, to_node=r2_params, from_idx=0, to_idx=0))
            pout_dims = ProvisionalDim(sizes[:-2:] + sizes[-1::])
            params = r2_params
        else:
            pout_dims = ProvisionalDim(sizes)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        all_nodes[node.output[0]] = (params, 0, pout_dims)
        return params

    @classmethod
    def version_10(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        scales = cls.get_constant(all_nodes[node.input[1]])
        return cls._common(node, scales, None, **kwargs)

    @classmethod
    def common_11_13(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[inp] if inp else None for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        nearest_mode = node.attrs.get('nearest_mode', 'round_prefer_floor')
        coord_transmode = node.attrs.get('coordinate_transformation_mode', 'half_pixel')
        if coord_transmode != 'align_corners':
            logger.warning('only align_corners is supported as coordinate_transformation_mode')

        scales_inp = inputs[2]
        scales_shape = scales_inp[2].shape if scales_inp else None
        if scales_inp and len(scales_shape) > 0 and scales_shape[0] == len(x_shape):
            scales = cls.get_constant(inputs[2])
            return cls._common(node, scales, None, nearest_mode=nearest_mode, **kwargs)
        else:
            sizes_inp = inputs[2]
            sizes_shape = sizes_inp[2].shape if sizes_inp else None
            if not sizes_inp or len(sizes_shape) < 1 or sizes_shape[0] == len(x_shape):
                raise ValueError('neither sizes nor scales input is valid for reshape')
            sizes = cls.get_constant(inputs[3])
            return cls._common(node, None, sizes, nearest_mode=nearest_mode, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls.common_11_13(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls.common_11_13(node, **kwargs)
