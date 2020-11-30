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

from graph.types import NNEdge, TransposeParameters
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.common.tflite_node import TFLiteNode
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("TRANSPOSE")
class Transpose(BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        new_axes = {}
        for idx, dim in enumerate(x_shape):
            if dim is not None:
                new_axes[idx] = len(new_axes)
        ptranspose = cls._verify_constant(inputs[1])
        pout_shape = [x_shape[dim] for dim in ptranspose]
        transpose = [new_axes[axis] for axis in ptranspose if x_shape[axis] is not None]
        node.input[1].used = True

        params = TransposeParameters(node.name, transpose=transpose)

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization([node.input[0]], node.output)
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pout_shape))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
