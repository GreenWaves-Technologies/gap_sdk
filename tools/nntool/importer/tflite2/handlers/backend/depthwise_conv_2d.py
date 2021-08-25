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
from graph.types import ConstantInputParameters, Conv2DParameters, NNEdge
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.common import LOG, check
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.tflite_schema_head.DepthwiseConv2DOptions import \
    DepthwiseConv2DOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .filter_mixin import FilterMixin


@tflite_op("DEPTHWISE_CONV_2D")
class DepthwiseConv2D(FilterMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(DepthwiseConv2DOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]

        x = inputs[0]
        x = cls.remove_known_batch_dimension(G, x, node)
        x_shape = x[2].shape
        in_b, h, w, in_c = tuple(x_shape)

        filt = inputs[1]
        weights_node = filt[0]
        filt_shape = filt[2].shape
        # ['in_c', 'h', 'w', 'out_c']
        filt_in_c, filt_h, filt_w, filt_out_c = tuple(filt_shape)

        # get filter dimensions
        if filt_h > h or filt_w > w:
            LOG.warning("Filter %s of shape [%dx%d] is bigger than input of shape [%dx%d]",
                        node.name, filt_h, filt_w, h, w)

        filt_dim = Conv2DFilterDim(filt_h, filt_w,
                                   filt_out_c, in_c=filt_in_c)
        filt_dim = filt_dim.impose_order(cls.TF_LITE_DW_FILTER_ORDER)

        # multiplier should match filter
        check(filt_dim.out_c == node_opts.DepthMultiplier() * in_c,
              "invalid multiplier")

        groups = filt_dim.out_c // node_opts.DepthMultiplier()

        # compute padding
        pad = cls.get_tf_padding(node_opts.Padding())

        # does it have biases
        if len(inputs) > 2:
            bias = inputs[2]
            bias_node = bias[0]
        else:
            bias_node = ConstantInputParameters(f'{node.name}_bias',
                                                dims=Dim.unnamed([filt_out_c]),
                                                value=np.zeros([filt_out_c], dtype=np.float32))  # TODO - check

        # TFLITE produces single channel input DW convolutions with the
        # multiplier equal to the number of out channels. This is just
        # a normal convolution and since we don't handle the channel
        # multiplier at present (but can) just convert them to normal
        # convolutions
        convert_to_conv = in_c == 1 and groups == 1

        if convert_to_conv:
            filt_dim.impose_order(cls.TF_LITE_FILTER_ORDER)
            params = Conv2DParameters(node.name,
                                      filt=filt_dim,
                                      stride=StrideDim(
                                          node_opts.StrideH(), node_opts.StrideW()),
                                      dilation=DilationDim(node_opts.DilationHFactor(),
                                                           node_opts.DilationWFactor()),
                                      padding=pad,
                                      has_bias=True,
                                      in_dims_hint=[['h', 'w', 'c'], cls.TF_LITE_FILTER_ORDER.copy(), [
                                          'out_c']],
                                      out_dims_hint=[['h', 'w', 'c']],
                                      constant_store=G.constant_store)
        else:
            filt_dim.impose_order(cls.TF_LITE_DW_FILTER_ORDER)
            params = Conv2DParameters(node.name,
                                      filt=filt_dim,
                                      stride=StrideDim(
                                          node_opts.StrideH(), node_opts.StrideW()),
                                      dilation=DilationDim(node_opts.DilationHFactor(),
                                                           node_opts.DilationWFactor()),
                                      padding=pad,
                                      groups=groups,
                                      multiplier=node_opts.DepthMultiplier(),
                                      has_bias=True,
                                      tf_depthwise=True,
                                      in_dims_hint=[['h', 'w', 'c'], cls.TF_LITE_DW_FILTER_ORDER.copy(), [
                                          'out_c']],
                                      out_dims_hint=[['h', 'w', 'c']],
                                      constant_store=G.constant_store)

        G.add_edge(NNEdge(from_node=weights_node, to_node=params, to_idx=1))
        G.add_edge(NNEdge(from_node=bias_node, to_node=params, to_idx=2))
        cls.new_load_filter_parameters(G, params, params.filter.actual_shape, params.filter.get_order_idx('out_c'),
                                       node.input[0], weights_node, bias_node,
                                       node.output[0], opts, dw_to_pw=convert_to_conv)

        in_dim = Dim.named_ordered(h=h, w=w, c=in_c)
        out_dims = params.get_output_size(
            [in_dim, Dim.unnamed(filt_dim.shape), Dim.unnamed([filt_out_c])])
        pout_dims = ProvisionalDim([in_b] + out_dims[0].shape)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        params = cls.fuse_activation(node_opts, node.name, params, **kwargs)
        all_nodes[node.output[0]] = (params, 0, pout_dims)
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_2(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
