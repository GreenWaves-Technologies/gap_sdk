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
from nntool.graph.types import NNEdge
from nntool.graph.types.conv2d import BatchNormalizationNode, Conv2DNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler
from ..handler import tflite_custom_op, tflite_op, handler_option

@handler_option(
    'fold_batchnorm',
    val_type=bool,
    default=True,
    desc="fold batch norm operations into filters where possible in TFLITE graphs"
)
@tflite_op("CUSTOM")
@tflite_custom_op("FlexFusedBatchNormV3")
class FlexFusedBatchNormV3(BackendHandler, ConstantMixin):

    @classmethod
    def _find_conv(cls, G, prev_node):
        if isinstance(prev_node, Conv2DNode):
            return prev_node
        return None

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        custom_opts = node.get_custom_options()
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']
        fold_batchnorm = kwargs['opts'].get('fold_batchnorm', True)

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        epsilon = custom_opts['epsilon']
        data_format = custom_opts['data_format']
        gamma = cls._verify_constant(inputs[1])
        beta = cls._verify_constant(inputs[2])
        running_mean = cls._verify_constant(inputs[3])
        running_variance = cls._verify_constant(inputs[4])
        conv = cls._find_conv(G, x[0])
        if fold_batchnorm and conv:
            conv_in_edges = G.indexed_in_edges(conv.name)
            weights_node = conv_in_edges[1].from_node
            biases_node = conv_in_edges[2].from_node
            weights = weights_node.value
            assert conv.has_bias, "convolution should always have bias even if zero"
            biases = biases_node.value
            # fold batch norm into conv weights and biases
            w_conv = weights.copy().reshape(weights.shape[0], -1)
            w_bn = np.diag(gamma / np.sqrt(epsilon + running_variance))
            w_conv = np.matmul(w_bn, w_conv).reshape(weights.shape)
            b_bn = beta + ((biases - running_mean) *  gamma  / np.sqrt(running_variance + epsilon))
            weights_node.value = w_conv
            biases_node.value = b_bn
            all_nodes[node.output[0]] = x
        else:
            params = BatchNormalizationNode(node.name, scale=gamma, bias=beta,
                                                running_mean=running_mean,
                                                running_variance=running_variance,
                                                momentum=0, epsilon=epsilon,
                                                axis=2 if data_format == "NHWC" else 0)
            if opts.get('load_quantization'):
                G.quantization[params.name] = cls.load_tf_quantization(
                    [node.input[0]], node.output)
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
            all_nodes[node.output[0]] = (params, 0, ProvisionalDim(x_shape))
            return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
