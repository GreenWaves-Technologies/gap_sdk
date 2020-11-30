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
from graph.types.base import NNEdge
from graph.types.conv2d import BatchNormalizationParameters, Conv2DParameters

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from importer.common.constant_mixin import ConstantMixin


@onnx_op("BatchNormalization")
class BatchNormalization(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        fold_batchnorm = kwargs['opts'].get('fold_batchnorm', True)

        x = inputs[0]
        x_shape = x[2].shape
        x_rank = len(x_shape)

        if x_rank > 4:
            raise ValueError("only 1D and 2D batch normalization is supported")

        momentum = node.attrs.get("momentum", 0.9)
        epsilon = node.attrs.get("epsilon", 0.9)

        bn_scale = cls.get_constant(inputs[1])
        bn_bias = cls.get_constant(inputs[2])
        running_mean = cls.get_constant(inputs[3])
        running_variance = cls.get_constant(inputs[4])

        # from version 7, force to use test mode
        if cls.SINCE_VERSION >= 7 or node.attrs.get("is_test", 0):
            spatial = None
        else:
            spatial = node.attrs.get("spatial", 1) == 1

        if fold_batchnorm and isinstance(x[0], Conv2DParameters):
            conv = x[0]
            weights = conv.weights
            if conv.has_bias:
                biases = conv.biases
            else:
                biases = np.zeros([weights.shape[0]])
                conv.has_bias = True
            # fold batch norm into conv weights and biases
            w_conv = weights.copy().reshape(weights.shape[0], -1)
            w_bn = np.diag(bn_scale / np.sqrt(epsilon + running_variance))
            w_conv = np.matmul(w_bn, w_conv).reshape(weights.shape)
            b_bn = bn_bias - bn_scale * running_mean / np.sqrt(running_variance + epsilon)
            conv.weights = w_conv
            conv.biases = biases + b_bn
            all_nodes[node.output[0]] = x
        else:
            params = BatchNormalizationParameters(valid_name, scale=bn_scale, bias=bn_bias,
                                                  running_mean=running_mean,
                                                  running_variance=running_variance, spatial=spatial,
                                                  momentum=momentum, epsilon=epsilon)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
            all_nodes[node.output[0]] = (params, 0, deepcopy(x[2]))
            return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_9(cls, node, **kwargs):
        return cls._common(node, **kwargs)
