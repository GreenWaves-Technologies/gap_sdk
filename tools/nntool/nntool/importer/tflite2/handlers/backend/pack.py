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
from nntool.graph.types import (ConcatNode, ConstantInputNode, NNEdge,
                         ReshapeNode)
from nntool.graph.types.misc import CopyNode
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import LOG, check
from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.tflite2.tflite_schema_head.PackOptions import PackOptions

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
        if any(not cls.is_constant(inp) for inp in inputs):
            seen_buf_idx = set()
            have_warned = False
            for idx, buf_idx in enumerate(buffer_idxes):
                if not buf_idx in seen_buf_idx:
                    seen_buf_idx.add(buf_idx)
                    continue
                if not have_warned:
                    LOG.warning(f'pack {node.name} has duplicate inputs. Inserting copies but this is not very efficient.')
                    have_warned = True
                copy = CopyNode(G.unique_name(f'{node.name}_copy{buf_idx}'))
                inp = inputs[idx]
                G.add_edge(NNEdge(from_node=inp[0], from_idx=inp[1], to_node=copy))
                inputs[idx] = (copy, 0) + inp[2:]

        axis = node_opts.Axis()
        # negative values wrap
        if axis < 0:
            axis += (len(inp_shapes[0]) + 1)

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
            params = ConstantInputNode(node.name, value=value)
        elif len(inputs) == 1:
            params = ReshapeNode(node.name,
                                        old_shape=reshape_in_shape,
                                        shape=concat_in_shape)
            G.add_edge(NNEdge(from_node=inputs[0][0], to_node=params, from_idx=inputs[0][1]))
        else:
            axis -= sum(1 if dim is None else 0 for dim in pconcat_out_shape[:axis:])
            params = ConcatNode(node.name, axis=axis)

            # insert reshapes on each input to add concat axis
            for idx, inp in enumerate(inputs):
                rparams = ReshapeNode(G.unique_name(f'{node.name}_reshape{idx}'),
                                            old_shape=reshape_in_shape,
                                            shape=concat_in_shape)
                G.add_edge(NNEdge(from_node=inp[0], to_node=rparams, from_idx=inp[1]))
                G.add_edge(NNEdge(from_node=rparams, to_node=params, to_idx=idx))
                if opts.get('load_quantization'):
                    G.quantization[rparams.name] = cls.load_tf_quantization([node.input[idx]], [node.input[idx]])

        if opts.get('load_quantization'):
            G.quantization[params.name] = cls.load_tf_quantization(node.input, node.output)

        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pconcat_out_shape))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
