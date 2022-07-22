# Copyright (C) 2022  GreenWaves Technologies, SAS

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


from nntool.graph.types import ConstantInputNode, NNEdge, StridedSliceNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import LOG
from nntool.importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler
from ..handler import tflite_op, partial_support, ps_description


@tflite_op("SLICE")
@partial_support(True)
@ps_description("start and size inputs must reduce to constants at import time")
class Slice(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        start = cls._verify_constant(inputs[1])
        size = tuple([sz if sz > 0 else x_shape[idx] - start[idx]
                      for idx, sz in enumerate(cls._verify_constant(inputs[2]))])

        act_slice = tuple([(s_dim, s_dim + sz, 1)
                           for idx, (s_dim, sz) in enumerate(zip(start, size)) if x_shape[idx]])
        pout_shape = ProvisionalDim(tuple([dim if x_shape[idx] else None
                                           for idx, dim in enumerate(size) if x_shape[idx]]))

        return cls.emit_slice(node, G, opts, all_nodes, x, act_slice, pout_shape)

    @classmethod
    def emit_slice(cls, node, G, opts, all_nodes, x, act_slice, pout_shape):
        if cls.is_constant(x):
            LOG.info("reducing %s to a constant", node.name)
            x_val = cls.get_constant(x)
            params = StridedSliceNode(
                node.name, act_slice=act_slice, out_shape=pout_shape.known_shape)
            x_val = params.numpy_slice(x_val)
            params = ConstantInputNode(node.name, value=x_val)
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        else:
            params = StridedSliceNode(
                node.name, act_slice=act_slice, out_shape=pout_shape.known_shape)
            G.add_edge(
                NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))

        if opts.get('load_quantization'):
            G.quantization[params.name] = cls.load_tf_quantization(
                [node.input[0]], node.output)
        all_nodes[node.output[0]] = (params, 0, pout_shape)
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
