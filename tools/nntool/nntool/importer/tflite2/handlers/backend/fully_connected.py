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
from nntool.graph.dim import Dim, FcFilterDim
from nntool.graph.types import (ConstantInputNode, LinearNode,
                         MatMulTransposedNode, NNEdge, ReshapeNode,
                         TransposeNode)
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import TFLiteImportException, check
from nntool.importer.tflite2.handlers.backend.filter_mixin import FilterMixin
from nntool.importer.tflite2.tflite_schema_head.FullyConnectedOptions import \
    FullyConnectedOptions

from ..backend_handler import BackendHandler
from ..handler import partial_support, ps_description, tflite_op


@tflite_op("FULLY_CONNECTED")
@partial_support(True)
@ps_description("weights_format and asymmetric_quantize_inputs parameters are ignored. keep_num_dims is not supported.")
class FullyConnected(FilterMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        node_opts = node.get_options(FullyConnectedOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        keep_dims = node_opts.KeepNumDims()
        # check(not keep_dims,
        #       f'keep dims on Fully Connected {node.name} is not supported')

        inputs = [all_nodes[t] if t is not None else None for t in node.input]

        x = inputs[0]
        x_shape = x[2]
        x_known_shape = x_shape.known_shape
        inp_sz = np.prod(np.array(x_known_shape))
        weights = inputs[1]
        weights_node = weights[0]
        weights_shape = weights[2].shape
        check(len(weights_shape) == 2,
              f'bad filter shape {weights_shape} in {node.name}')
        out_c = weights_shape[0]
        batch_size = inp_sz // weights_shape[1]

        keep_dims = node_opts.KeepNumDims()
        if keep_dims:
            if x_shape.shape[-1] != weights_shape[1]:
                raise TFLiteImportException(
                    f'Keep dims set on {node.name} but last input dimension does not match weights')
            out_shape = x_shape.shape.copy()
            out_shape[-1] = out_c
        elif batch_size > 1:
            out_shape = (batch_size, out_c)
        else:
            out_shape = (None, out_c)
        real_out_shape = tuple(dim for dim in out_shape if dim is not None)

        filt_dim = FcFilterDim(weights_shape[0], weights_shape[1])

        node.input[1].used = True
        check(filt_dim.sz * batch_size == inp_sz,
              "filter doesn't match input size")

        if len(inputs) > 2 and inputs[2] is not None:
            bias = inputs[2]
            bias_node = bias[0]
        else:
            bias_node = ConstantInputNode(f'{node.name}_bias',
                                                dims=Dim.unnamed([out_c]),
                                                value=np.zeros([out_c],
                                                               dtype=np.float32))

        if batch_size > 1:
            # add a reshape to force the size of the input to batch * in_c
            input_shape = (batch_size, weights_shape[1])
            if x_known_shape != input_shape:
                rparams = ReshapeNode(
                    G.unique_name(f'{node.name}_batch'),
                    old_shape=Dim.unnamed(x_known_shape),
                    shape=Dim.unnamed(input_shape))
                G.add_edge(
                    NNEdge(from_node=x[0], to_node=rparams, from_idx=x[1], to_idx=0))
                link = (rparams, 0)
            else:
                link = x

            # the batched linear is ([NxM] . [MxK]) + [K]
            params = MatMulTransposedNode(node.name)
            cls.new_load_filter_parameters(G, params, weights_shape, 0,
                                           node.input[0], weights_node,
                                           bias_node, node.output[0], opts)
            trans2 = TransposeNode(G.unique_name(
                f'{node.name}_tin2'), transpose=(1, 0))
            G.add_edge(
                NNEdge(from_node=link[0], to_node=params, from_idx=link[1]))
            G.add_edge(NNEdge(from_node=weights_node,
                              to_node=params, to_idx=1))
            #G.add_edge(NNEdge(from_node=trans2, to_node=params, to_idx=1))
            G.add_edge(NNEdge(from_node=bias_node, to_node=params, to_idx=2))
            fc_shape = (batch_size, out_c)
        else:
            ker_in_order = None
            ker_out_order = None
            link = (x[0], x[1])

            params = LinearNode(node.name, filter_dim=filt_dim, has_bias=True,
                                  ker_in_order=ker_in_order,
                                  ker_out_order=ker_out_order,
                                  batch_size=batch_size,
                                  keep_dims=keep_dims)
            cls.new_load_filter_parameters(G, params, params.filter_dim.actual_shape, params.filter_dim.get_order_idx('out_c'),
                                           node.input[0], weights_node,
                                           bias_node, node.output[0], opts)

            G.add_edge(NNEdge(from_node=weights_node,
                              to_node=params, to_idx=1))
            G.add_edge(NNEdge(from_node=bias_node, to_node=params, to_idx=2))
            G.add_edge(NNEdge(from_node=link[0], to_node=params,
                              from_idx=link[1], to_idx=0))
            fc_shape = (out_c,)

        pout_dims = ProvisionalDim(out_shape)
        aparams = cls.fuse_activation(node_opts, node.name, params, **kwargs)

        if real_out_shape != fc_shape:
            rparams = ReshapeNode(G.unique_name(f'{node.name}_keepdims'),
                                        old_shape=fc_shape, shape=real_out_shape)
            G.add_edge(NNEdge(from_node=aparams, to_node=rparams))
            aparams = rparams

        all_nodes[node.output[0]] = (aparams, 0, pout_dims)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_2(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_5(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)
