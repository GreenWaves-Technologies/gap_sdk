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
from graph.dim import Conv2DFilterDim, DilationDim, Dim, StrideDim
from graph.types.base import NNEdge
from graph.types.conv2d import Conv2DParameters
from importer.common.broadcast_mixin import BroadcastMixin
from importer.common.provisional_dim import ProvisionalDim
from utils.sparse_list import SparseList
from importer.onnx.common import logger
from ..handler import partial_support, ps_description
from .pad_mixin import PadMixin
from importer.common.constant_mixin import ConstantMixin


@partial_support(True)
@ps_description("Supports only 1D and 2D convolutions. Supports only constant weights.")
class ConvMixin(BroadcastMixin, PadMixin, ConstantMixin):
    ONNX_FILTER_ORDER = ['out_c', 'in_c', 'h', 'w']

    @classmethod
    def conv(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        # input N x C x H x W
        x = inputs[0]
        x_rank = len(x[2].shape)
        x_shape = x[2].shape
        spatial_size = x_rank - 2
        assert spatial_size <= 2, "only 1D and 2D convolutions supported"

        # M x C/group x kH x kW
        weights = cls.get_constant(inputs[1])
        out_c = weights.shape[0]
        group = node.attrs.get("group", 1)
        in_c = x_shape[1]
        filt_in_c = in_c // group
        filt_h = weights.shape[2]
        filt_w = weights.shape[2]
        h = 1 if spatial_size <= 1 else x_shape[2]
        w = 1 if spatial_size == 0 else (x_shape[2] if spatial_size == 1 else x_shape[3])

        filt_dim = Conv2DFilterDim(filt_h, filt_w,
                                   out_c, in_c=filt_in_c)
        filt_dim = filt_dim.impose_order(cls.ONNX_FILTER_ORDER)

        if len(inputs) > 2:
            biases = cls.get_constant(inputs[2])
        else:
            biases = np.zeros([out_c])

        dilations = cls.pad_start_with(node.attrs.get("dilations", [1] * spatial_size), [1], 2)
        strides = cls.pad_start_with(node.attrs.get("strides", [1] * spatial_size), [1], 2)
        pad_dim = cls.calc_pad_dim(node, spatial_size)

        params = Conv2DParameters(valid_name,
                                  filt=filt_dim,
                                  stride=StrideDim(strides[0],
                                                   strides[1]),
                                  dilation=DilationDim(dilations[0],
                                                       dilations[1]),
                                  groups=group,
                                  padding=pad_dim,
                                  has_bias=True,
                                  in_dims_hint=SparseList([['c', 'h', 'w']]),
                                  out_dims_hint=SparseList([['c', 'h', 'w']]),
                                  constant_store=G.constant_store)
        params.weights = weights
        params.biases = biases
        in_dim = Dim.named_ordered(c=in_c, h=h, w=w)
        out_dims = params.get_output_size([in_dim])
        pout_dims = ProvisionalDim([x_shape[0]] + out_dims[0].shape)
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pout_dims)
        return params
