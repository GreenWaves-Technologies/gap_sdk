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


from copy import deepcopy

import numpy as np
from graph.dim import Conv2DFilterDim, DilationDim, Dim, StrideDim
from graph.types.base import NNEdge
from graph.types.conv2d import Conv2DParameters
from graph.types.input_output import ConstantInputParameters
from graph.types.others import ReshapeParameters
from importer.common.broadcast_mixin import BroadcastMixin
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..handler import partial_support, ps_description
from .pad_mixin import PadMixin


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
        if x_shape[0] is not None and x_shape[0] > 1:
            batch = x_shape[0]
            logger.warning(f"{valid_name} has a non 1 batch dimension of {batch} - this is not supported by nntool or autotiler kernels")
        else:
            batch = None
        real_in_shape = deepcopy(x_shape)
        #conv_shape = [x if idx > 0 and x is not None else 1 for idx, x in enumerate(x_shape)]
        conv_shape = x_shape
        if None in x_shape:
            real_in_shape.remove(None)
        spatial_size = x_rank - 2
        assert spatial_size == 2 or spatial_size == 1, "only 1D and 2D convolutions supported"

        # M x C/group x kH x kW
        weights_node = inputs[1][0]
        weights_node.name = f'{valid_name}_weights'
        weights = cls.get_constant(inputs[1])
        out_c = weights.shape[0]
        group = node.attrs.get("group", 1)
        in_c = conv_shape[-spatial_size-1] if conv_shape[-spatial_size-1] is not None else 1
        filt_in_c = in_c // group
        if in_c != weights.shape[1] * group:
            raise ValueError(f'node {valid_name} has incorrect input channel '
                             f'dimension {in_c} expecting {weights.shape[1] * group}')
        if spatial_size == 1:
            filt_w = weights.shape[-1]
            filt_h = 1
            # create a new constant node since we are changing the shape
            weights = np.reshape(weights, (out_c, filt_in_c, filt_h, filt_w))
            weights_node = ConstantInputParameters(f'{valid_name}_weights', value=weights,
                                                   dims=Dim.unnamed(
                                                       weights.shape),
                                                   constant_store=G.constant_store)
        else:
            filt_h = weights.shape[-2]
            filt_w = weights.shape[-1]
        h = 1 if spatial_size == 1 else (conv_shape[-2] if conv_shape[-2] is not None else 1)
        w = conv_shape[-1] if conv_shape[-1] is not None else 1

        filt_dim = Conv2DFilterDim(filt_h, filt_w,
                                   out_c, in_c=filt_in_c)
        filt_dim = filt_dim.impose_order(cls.ONNX_FILTER_ORDER)

        if len(inputs) > 2:
            biases_node = inputs[2][0]
            biases = cls.get_constant(inputs[2])
        else:
            biases = np.zeros([out_c], dtype=np.float32)
            biases_node = ConstantInputParameters(f'{valid_name}_biases', value=biases,
                                                  dims=Dim.unnamed(
                                                      biases.shape),
                                                  constant_store=G.constant_store)

        dilations = cls.pad_start_with(node.attrs.get("dilations", []), [1], 2)
        strides = cls.pad_start_with(node.attrs.get("strides", []), [1], 2)
        pad_dim = cls.calc_pad_dim(node, 4)

        if batch is not None:
            in_hint = ['n', 'c', 'h', 'w']
            out_hint = ['n', 'c', 'h', 'w']
            in_dim = Dim.named_ordered(n=batch, c=in_c, h=h, w=w)
            ker_in_order = [
                ['n', 'c', 'h', 'w'],
                ['out_c', 'in_c', 'h', 'w'],
                ['out_c']]
            ker_out_order = [['n', 'c', 'h', 'w']]
        else:
            in_hint = ['c', 'h', 'w']
            out_hint = ['c', 'h', 'w']
            in_dim = Dim.named_ordered(c=in_c, h=h, w=w)
            ker_in_order = [
                ['c', 'h', 'w'],
                ['out_c', 'in_c', 'h', 'w'],
                ['out_c']]
            ker_out_order = [['c', 'h', 'w']]
        params = Conv2DParameters(valid_name,
                                  filt=filt_dim,
                                  stride=StrideDim(strides[0],
                                                   strides[1]),
                                  dilation=DilationDim(dilations[0],
                                                       dilations[1]),
                                  batch=batch,
                                  groups=group,
                                  padding=pad_dim,
                                  ker_in_order=ker_in_order,
                                  ker_out_order=ker_out_order,
                                  has_bias=True,
                                  in_dims_hint=[in_hint,
                                                cls.ONNX_FILTER_ORDER, ['c']],
                                  out_dims_hint=[out_hint],
                                  constant_store=G.constant_store)

        w_dim = Dim.named_ordered(
            out_c=out_c, in_c=filt_in_c, h=filt_h, w=filt_w)
        b_dim = Dim.named_ordered(c=out_c)
        out_dims = params.get_output_size([in_dim, w_dim, b_dim])
        G.add_edge(NNEdge(from_node=weights_node,
                          to_node=params, from_idx=0, to_idx=1))
        G.add_edge(NNEdge(from_node=biases_node,
                          to_node=params, from_idx=0, to_idx=2))
        if conv_shape != real_in_shape:
            # insert reshape from [xx,None,xx,xx] -> [None, xx, xx, xx]
            rbatch_params = ReshapeParameters(f'{valid_name}_reshape_batchdim',
                                          old_shape=Dim.unnamed(conv_shape),
                                          shape=Dim.unnamed(real_in_shape))
            G.add_edge(
                NNEdge(from_node=x[0], to_node=rbatch_params, from_idx=x[1], to_idx=0))
            prev_node = rbatch_params
            prev_idx = 0
        else:
            prev_node = x[0]
            prev_idx = x[1]

        if spatial_size == 1:
            if batch is not None:
                oned_in_shape = [batch, in_c, w]
                twod_in_shape = [batch, in_c, 1, w]
                oned_out_shape = [batch, out_dims[0].c, out_dims[0].w]
                pout_dims = ProvisionalDim(oned_out_shape)
            else:
                oned_in_shape = [in_c, w]
                twod_in_shape = [in_c, 1, w]
                oned_out_shape = [out_dims[0].c, out_dims[0].w]
                pout_dims = ProvisionalDim([conv_shape[0]] + oned_out_shape)
            r1_params = ReshapeParameters(f'{valid_name}_reshape2d',
                                          old_shape=Dim.unnamed(oned_in_shape),
                                          shape=Dim.unnamed(twod_in_shape))
            r2_params = ReshapeParameters(f'{valid_name}_reshape1d',
                                          old_shape=out_dims[0],
                                          shape=Dim.unnamed(oned_out_shape))
            G.add_edge(
                NNEdge(from_node=prev_node, to_node=r1_params, from_idx=prev_idx, to_idx=0))
            G.add_edge(NNEdge(from_node=r1_params,
                              to_node=params, from_idx=0, to_idx=0))
            G.add_edge(NNEdge(from_node=params,
                              to_node=r2_params, from_idx=0, to_idx=0))
            all_nodes[node.output[0]] = (r2_params, 0, pout_dims)
            return r2_params
        else:
            pout_dims = ProvisionalDim([conv_shape[0]] + out_dims[0].shape)
            G.add_edge(
                NNEdge(from_node=prev_node, to_node=params, from_idx=prev_idx, to_idx=0))
            all_nodes[node.output[0]] = (params, 0, pout_dims)
            return params
