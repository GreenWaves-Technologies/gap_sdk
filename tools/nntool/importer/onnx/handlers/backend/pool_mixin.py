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

from graph.dim import Dim, PoolFilterDim, StrideDim
from graph.types import GlobalPoolParameters, PoolingParameters
from graph.types.base import NNEdge
from importer.common.provisional_dim import ProvisionalDim

from ..handler import partial_support, ps_description
from .pad_mixin import PadMixin


@partial_support(True)
@ps_description("Supports only 2D pools.")
class PoolMixin(PadMixin):

    @classmethod
    def pool(cls, node, pool_type=None, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        x_feature_shape = x_shape[2::]
        in_c = x_shape[1]

        kernel_shape = node.attrs["kernel_shape"]
        spatial_size = len(kernel_shape)
        x_rank = spatial_size + 2
        if spatial_size != 2:
            raise ValueError(valid_name + " with {}D input".format(x_rank))

        h = x_shape[2]
        w = x_shape[3]

        strides = node.attrs.get("strides", [1] * spatial_size)
        stride_is_one = all(stride == 1 for stride in strides)
        dilations = node.attrs.get("dilations", [1] * spatial_size)
        if any(dilation > 1 for dilation in dilations):
            raise ValueError(valid_name + " with dilation not supported")
        # ceil_mode = bool(node.attrs.get("ceil_mode", 0))
        pad_dim = cls.calc_pad_dim(node, spatial_size)
        # Note: This needs to check dilation if it is added
        filter_matches_input = (all(k_dim >= (x_dim + pad) for k_dim, x_dim, pad
                                    in zip(kernel_shape, x_feature_shape, [pad_dim.h, pad_dim.w])))

        if filter_matches_input and stride_is_one:
            params = GlobalPoolParameters(valid_name,
                                          pool_type=pool_type,
                                          axis=[1, 2],
                                          keep_dims=True,
                                          in_dims_hint=[['c', 'h', 'w']],
                                          out_dims_hint=[['c', 'h', 'w']])
        else:
            params = PoolingParameters(valid_name,
                                       filt=PoolFilterDim(kernel_shape[0],
                                                          kernel_shape[1]),
                                       stride=StrideDim(strides[0],
                                                        strides[1]),
                                       padding=pad_dim,
                                       pool_type=pool_type,
                                       in_dims_hint=[['c', 'h', 'w']],
                                       out_dims_hint=[['c', 'h', 'w']])

        in_dim = Dim.named_ordered(c=in_c, h=h, w=w)
        out_dims = params.get_output_size([in_dim])
        pout_dims = ProvisionalDim([x_shape[0]] + out_dims[0].shape)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pout_dims)
        return params
