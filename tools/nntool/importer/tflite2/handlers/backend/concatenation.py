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

from functools import reduce

import numpy as np
from graph.types import ConcatParameters, ConstantInputParameters, NNEdge
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.common import LOG
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.tflite_schema_head.ConcatenationOptions import \
    ConcatenationOptions
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("CONCATENATION")
class Concatenation(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(ConcatenationOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        inp_shapes = [input[2].shape for input in inputs]

        buffer_idxes = [tensor.buffer_idx for tensor in node.input]
        non_zero_idxes = [idx for idx in buffer_idxes if idx != 0]
        if len(set(non_zero_idxes)) != len(non_zero_idxes):
            raise NotImplementedError(
                "concats with multiple versions of the same input are not supported. "
                "This is normally a graph design problem.")

        axis = node_opts.Axis()
        if any(inp_shape[axis] is None for inp_shape in inp_shapes):
            raise ValueError("concat on undefined axis in node %s" % node.name)

        def red_func(x, y):
            return y.copy() if x is None else [elem if idx != axis else elem + y[axis]
                                               for idx, elem in enumerate(x)]

        pout_shape = reduce(red_func, inp_shapes)
        cls.remove_none_from_constants(inputs, pout_shape)

        if all(cls.is_constant(inp) for inp in inputs):
            LOG.info("reducing %s to a constant", node.name)
            value = np.concatenate([cls.get_constant(inp) for inp in inputs], axis=axis)
            params = ConstantInputParameters(node.name, value=value, constant_store=G.constant_store)
        else:
            axis -= sum(1 if dim is None else 0 for dim in pout_shape[:axis:])
            params = ConcatParameters(node.name, axis=axis, axis_hint=None)

            for idx, inp in enumerate(inputs):
                G.add_edge(NNEdge(from_node=inp[0], to_node=params, from_idx=inp[1], to_idx=idx))
        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization(node.input, node.output)
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pout_shape))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
