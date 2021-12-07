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
from graph.dim import Dim
from graph.types import ConstantInputParameters, NNEdge, ReshapeParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.common import LOG
from importer.tflite2.common.tflite_node import TFLiteNode
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("EXPAND_DIMS")
class ExpandDims(BackendHandler, ConstantMixin):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        exp_dim = int(cls._verify_constant(inputs[1]))
        if exp_dim < 0:
            exp_dim += len(x_shape)
        if x_shape[exp_dim] is None:
            exp_dim += 1
        new_shape = x_shape[:exp_dim:] + [1] + x_shape[exp_dim::]

        if cls.is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            val = np.reshape(cls.get_constant(x), new_shape)
            params = ConstantInputParameters(node.name, value=val,
                                             dims=Dim.unnamed(val.shape))
        else:
            pnew_shape = ProvisionalDim(new_shape)
            old_shape = Dim.unnamed(
                cls.remove_unspecified_dim(x_shape), is_ordered=True)
            new_shape = Dim.unnamed(
                cls.remove_unspecified_dim(new_shape), is_ordered=True)
            params = ReshapeParameters(
                node.name, old_shape=old_shape, shape=new_shape)

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization(
                [node.input[0]], node.output)
        G.add_edge(
            NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pnew_shape)
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
