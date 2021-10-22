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
from graph.dim import Conv2DFilterDim, DilationDim, Dim, Dim2D, PadDim, StrideDim
from graph.types import ConstantInputParameters, TransposeConv2DParameters, ReshapeParameters
from graph.types.base import NNEdge
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from .pad_mixin import PadMixin

# output_shape[i] = stride[i] * (input_size[i] - 1) + output_padding[i] + ((kernel_shape[i] - 1) * dilations[i] + 1) - pads[start_i] - pads[end_i]

def expand_dim(shape, len, val):
    if shape is None:
        return None
    return ([val] * len) + list(shape)

@onnx_op("ConvTranspose")
class ConvTranspose(BackendHandler, ConstantMixin):
    ONNX_TRANSFILTER_ORDER = ['in_c', 'out_c', 'h', 'w']

    @staticmethod
    def calc_shapes(node, spatial_size, input_size, kernel_shape):
        padding = expand_dim(node.attrs.get('pads', None), 4 - spatial_size * 2, 0)
        auto_pad = node.attrs.get('auto_pad', 'NOTSET')
        output_shape = expand_dim(node.attrs.get('output_shape', None), 2 - spatial_size, 1)
        output_padding = Dim2D(*expand_dim(node.attrs.get('output_padding', None), 2 - spatial_size, 0))
        dilations = DilationDim(*expand_dim(node.attrs.get('dilations', None), 2 - spatial_size, 1))
        strides = StrideDim(*expand_dim(node.attrs.get('strides', None), 2 - spatial_size, 1))
        if output_shape:
            total_padding = strides * (input_size - 1) + output_padding + ((kernel_shape - 1) * dilations + 1) - output_shape
            if auto_pad == 'SAME_UPPER':
                pad_start = total_padding // 2
                pad_end = total_padding - pad_start
            else:
                pad_end = total_padding // 2
                pad_start = total_padding - pad_end
            padding = PadDim(pad_start.h, pad_end.h, pad_start.w, pad_end.w)
        elif auto_pad == 'NOTSET':
            assert padding, 'pads not set and auto_pad is NOTSET'
            padding = PadDim(*padding)
        elif auto_pad == 'VALID':
            padding = PadDim.valid()
        return padding, dilations, strides, output_padding
            

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        # input N x C x H x W
        x = inputs[0]
        x_rank = len(x[2].shape)
        x_shape = x[2].shape
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
        out_c = weights.shape[1]
        group = node.attrs.get("group", 1)
        in_c = conv_shape[-spatial_size-1] if conv_shape[-spatial_size-1] is not None else 1
        filt_out_c = out_c // group
        if in_c != weights.shape[0]:
            raise ValueError(f'node {valid_name} has incorrect input channel '
                             f'dimension {in_c} expecting {weights.shape[0]}')
        if spatial_size == 1:
            filt_w = weights.shape[-1]
            filt_h = 1
            # create a new constant node since we are changing the shape
            weights = np.reshape(weights, (in_c, filt_out_c, filt_h, filt_w))
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
                                   filt_out_c, in_c=in_c)
        filt_dim = filt_dim.impose_order(cls.ONNX_TRANSFILTER_ORDER)

        if len(inputs) > 2:
            biases_node = inputs[2][0]
            biases = cls.get_constant(inputs[2])
        else:
            biases = np.zeros([out_c], dtype=np.float32)
            biases_node = ConstantInputParameters(f'{valid_name}_biases', value=biases,
                                                  dims=Dim.unnamed(
                                                      biases.shape),
                                                  constant_store=G.constant_store)

        padding, dilations, strides, output_padding = cls.calc_shapes(node, spatial_size, Dim2D((h, w)), Dim2D((filt_h, filt_w)))

        params = TransposeConv2DParameters(valid_name,
                                  filt=filt_dim,
                                  stride=strides,
                                  dilation=dilations,
                                  groups=group,
                                  padding=padding,
                                  has_bias=True,
                                  in_dims_hint=[['c', 'h', 'w'],
                                                cls.ONNX_TRANSFILTER_ORDER, ['c']],
                                  out_dims_hint=[['c', 'h', 'w']],
                                  constant_store=G.constant_store)

        in_dim = Dim.named_ordered(c=in_c, h=h, w=w)
        w_dim = Dim.named_ordered(
            out_c=filt_out_c, in_c=in_c, h=filt_h, w=filt_w)
        b_dim = Dim.named_ordered(c=out_c)
        out_dims = params.get_output_size([in_dim, w_dim, b_dim])
        G.add_edge(NNEdge(from_node=weights_node,
                          to_node=params, from_idx=0, to_idx=1))
        G.add_edge(NNEdge(from_node=biases_node,
                          to_node=params, from_idx=0, to_idx=2))
        if conv_shape != real_in_shape:
            # insert reshape from [xx,None,xx,xx] -> [None, xx, xx, xx]
            rbatch_params = ReshapeParameters(G.unique_name(f'{valid_name}_reshape_batchdim'),
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
            oned_in_shape = [in_c, w]
            twod_in_shape = [in_c, 1, w]
            oned_out_shape = [out_dims[0].c, out_dims[0].w]
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
            pout_dims = ProvisionalDim([conv_shape[0]] + oned_out_shape)
            all_nodes[node.output[0]] = (r2_params, 0, pout_dims)
            return r2_params
        else:
            pout_dims = ProvisionalDim([conv_shape[0]] + out_dims[0].shape)
            G.add_edge(
                NNEdge(from_node=prev_node, to_node=params, from_idx=prev_idx, to_idx=0))
            all_nodes[node.output[0]] = (params, 0, pout_dims)
            return params


    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)
