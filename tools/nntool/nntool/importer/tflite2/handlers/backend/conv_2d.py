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
from nntool.graph.dim import Conv2DFilterDim, DilationDim, Dim, StrideDim
from nntool.graph.types import ConstantInputNode, Conv2DNode, NNEdge
from nntool.graph.types.tensor_shape import ReshapeNode
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import LOG
from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.tflite2.tflite_schema_head.Conv2DOptions import Conv2DOptions
from nntool.quantization.new_qrec import QRec

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .filter_mixin import FilterMixin


@tflite_op("CONV_2D")
class Conv2D(FilterMixin, BackendHandler):

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(Conv2DOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]

        x = inputs[0]
        # x = cls.remove_known_batch_dimension(G, x, node)
        x_shape = x[2].shape
        batch, h, w, in_c = tuple(x_shape)

        ker_in_order = [
            ['c', 'h', 'w'],
            ['out_c', 'in_c', 'h', 'w'],
            ['out_c']]
        ker_out_order = [['c', 'h', 'w']]
        inout_hint = ['h', 'w', 'c']
        if batch is not None:
            if batch > 1:
                # support for multi batch is very limited
                LOG.warning(
                    f"{node.name} has a non 1 batch dimension of {batch} -"
                    " this is not supported by nntool or autotiler kernels")
                inout_hint = ['n', 'h', 'w', 'c']
                ker_in_order = [
                    ['n', 'c', 'h', 'w'],
                    ['out_c', 'in_c', 'h', 'w'],
                    ['out_c']]
                ker_out_order = [['n', 'c', 'h', 'w']]
            else:
                new_shape = x_shape[1:]
                reshape = ReshapeNode(
                    G.unique_name(f'{node.name}_batch'),
                    old_shape=x_shape,
                    shape=new_shape
                )
                if G.quantization:
                    qrec = G.quantization[x[0].name]
                    G.quantization[reshape.name] = QRec.copy_ktype(
                        qrec,
                        in_qs=[qrec.out_qs[0]],
                        out_qs=[qrec.out_qs[0]])
                G.add_edge(NNEdge(from_node=x[0], from_idx=x[1], to_node=reshape))
                x_shape = new_shape
                x = (reshape, 0)
                batch = None

        filt = inputs[1]
        weights_node = filt[0]
        filt_shape = filt[2].shape
        # ['in_c', 'h', 'w', 'out_c']
        filt_out_c, filt_h, filt_w, filt_in_c = tuple(filt_shape)

        # get filter dimensions
        if filt_h > h or filt_w > w:
            LOG.warning("Filter %s of shape [%dx%d] is bigger than input of shape [%dx%d]",
                        node.name, filt_h, filt_w, h, w)

        filt_dim = Conv2DFilterDim(filt_h, filt_w,
                                   filt_out_c, in_c=filt_in_c)
        filt_dim = filt_dim.impose_order(cls.TF_LITE_FILTER_ORDER)

        # compute padding
        pad = cls.get_tf_padding(node_opts.Padding())

        # does it have biases
        if len(inputs) > 2:
            bias = inputs[2]
            bias_node = bias[0]
        else:
            bias_node = ConstantInputNode(
                f'{node.name}_bias',
                dims=Dim.unnamed([filt_out_c]),
                value=np.zeros([filt_out_c], dtype=np.float32))  # TODO - check
        groups = in_c // filt_in_c
        params = Conv2DNode(
            node.name,
            filter_dim=filt_dim,
            stride=StrideDim(
                node_opts.StrideH(), node_opts.StrideW()),
            dilation=DilationDim(node_opts.DilationHFactor(),
                                 node_opts.DilationWFactor()),
            groups=groups,
            padding=pad,
            batch=batch,
            has_bias=True,
            in_dims_hint=[inout_hint, cls.TF_LITE_FILTER_ORDER.copy(), [
                'out_c']],
            out_dims_hint=[inout_hint],
            ker_in_order=ker_in_order,
            ker_out_order=ker_out_order)
        G.add_edge(NNEdge(from_node=weights_node, to_node=params, to_idx=1))
        G.add_edge(NNEdge(from_node=bias_node, to_node=params, to_idx=2))
        cls.new_load_filter_parameters(
            G, params, params.filter_dim.actual_shape, params.filter_dim.get_order_idx(
                'out_c'),
            node.input[0], weights_node, bias_node, node.output[0], opts)

        if batch:
            in_dim = Dim.named_ordered(n=batch, h=h, w=w, c=in_c)
        else:
            in_dim = Dim.named_ordered(h=h, w=w, c=in_c)
        out_dims = params.get_output_size(
            [in_dim, Dim.unnamed(filt_dim.shape), Dim.unnamed([filt_out_c])])
        out_shape = out_dims[0].shape
        if batch is None:
            out_shape = [None] + out_shape
        pout_shape = ProvisionalDim(out_shape)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        oparams = cls.fuse_activation(node_opts, node.name, params, **kwargs)
        all_nodes[node.output[0]] = (oparams, 0, pout_shape)
        return oparams
