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
from graph.types import GlobalPoolingParameters, PoolingParameters
from graph.types.base import NNEdge
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.tflite_schema_head.Pool2DOptions import Pool2DOptions
from utils.node_id import NodeId

from .filter_pad_mixin import FilterPadMixin


class PoolMixin(FilterPadMixin):

    @classmethod
    def pool2d(cls, node, pool_type=None, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        opts = kwargs['opts']
        node_opts = node.get_options(Pool2DOptions)

        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x = cls.remove_known_batch_dimension(G, x, node)
        x_shape = x[2].shape
        in_c = x_shape[1]

        in_b, h, w, in_c = tuple(x_shape)

        filt_h = node_opts.FilterHeight()
        filt_w = node_opts.FilterWidth()
        stride_h = node_opts.StrideH()
        stride_w = node_opts.StrideW()

        pad = cls.get_tf_padding(node_opts.Padding())

        filter_matches_input = h == filt_h and w == filt_w
        stride_is_one = stride_h == 1 and stride_w == 1

        if filter_matches_input and stride_is_one:
            params = GlobalPoolingParameters(node.name,
                                          pool_type=pool_type,
                                          axis=[0, 1],
                                          keep_dims=True,
                                          in_dims_hint=[['h', 'w', 'c']],
                                          out_dims_hint=[['h', 'w', 'c']])
        else:
            params = PoolingParameters(node.name,
                                       filt=PoolFilterDim(filt_h, filt_w),
                                       stride=StrideDim(stride_h, stride_w),
                                       padding=pad,
                                       pool_type=pool_type,
                                       in_dims_hint=[['h', 'w', 'c']],
                                       out_dims_hint=[['h', 'w', 'c']])

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization(
                node.input, node.output)

        in_dim = Dim.named_ordered(h=h, w=w, c=in_c)
        out_dims = params.get_output_size([in_dim])
        pout_dims = ProvisionalDim([in_b] + out_dims[0].shape)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        params = cls.fuse_activation(node_opts, node.name, params, **kwargs)
        all_nodes[node.output[0]] = (params, 0, pout_dims)
        return params
