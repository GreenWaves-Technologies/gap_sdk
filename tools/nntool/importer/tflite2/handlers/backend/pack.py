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

from importer.common.constant_mixin import ConstantMixin
import numpy as np
from graph.types import ConcatParameters, NNEdge
from graph.types.input_output import ConstantInputParameters
from graph.types.others import ReshapeParameters
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.common import LOG, check
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.tflite_schema_head.PackOptions import PackOptions
from utils.node_id import NodeId

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("PACK")
class Pack(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(PackOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        all_nodes = kwargs['all_nodes']

        inputs = [all_nodes[t] for t in node.input]
        inp_shapes = [input[2].shape for input in inputs]

        values_count = node_opts.ValuesCount()
        check(len(inputs) == values_count, "invalid tflite file - values count not equal to inputs")

        buffer_idxes = [tensor.buffer_idx for tensor in node.input]
        check(len(set(buffer_idxes)) == len(buffer_idxes),
              "packs with multiple versions of the same input are not supported. This is normally a graph design problem.")

        axis = node_opts.Axis()
        dimension_size = len(inp_shapes)
        if axis < 0:
            axis += dimension_size

        check(all(shape == inp_shapes[0] for shape in inp_shapes[1::]),
              "invalid tflite file - pack inputs not the same")

        # prepare shapes of all tensors
        pconcat_out_shape = inp_shapes[0].copy()
        pconcat_out_shape.insert(axis, values_count)

        pconcat_in_shape = inp_shapes[0].copy()
        pconcat_in_shape.insert(axis, 1)

        preshape_in_shape = inp_shapes[0].copy()

        # remove nones from constants
        cls.remove_none_from_constants(inputs, preshape_in_shape)

        # remove nones from reshape shapes
        reshape_in_shape = cls.remove_unspecified_dim(preshape_in_shape)
        concat_in_shape = cls.remove_unspecified_dim(pconcat_in_shape)

        if all(cls.is_constant(inp) for inp in inputs):
            LOG.info("reducing %s to a constant", node.name)
            value = np.stack([cls.get_constant(inp) for inp in inputs], axis=axis)
            params = ConstantInputParameters(node.name, value=value, constant_store=G.constant_store)
        else:
            axis -= sum(1 if dim is None else 0 for dim in pconcat_out_shape[:axis:])
            params = ConcatParameters(node.name, axis=axis, axis_hint=None)

            # insert reshapes on each input to add concat axis
            for idx, inp in enumerate(inputs):
                rparams = ReshapeParameters(node.name + "_%s" % idx,
                                            old_shape=reshape_in_shape,
                                            shape=concat_in_shape)
                G.add_edge(NNEdge(from_node=inp[0], to_node=rparams, from_idx=inp[1]))
                G.add_edge(NNEdge(from_node=rparams, to_node=params, to_idx=idx))
                if opts.get('load_quantization'):
                    G.quantization[NodeId(rparams)] = cls.load_tf_quantization([node.input[idx]], [node.input[idx]])

        if opts.get('load_quantization'):
            G.quantization[NodeId(params)] = cls.load_tf_quantization(node.input, node.output)

        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pconcat_out_shape))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
