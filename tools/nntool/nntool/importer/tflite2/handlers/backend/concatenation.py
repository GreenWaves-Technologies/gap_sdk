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

import typing
from collections import Counter

import numpy as np
from nntool.graph.types import (ConcatNode, ConstantInputNode, CopyNode,
                                NNEdge, ReshapeNode)
from nntool.importer.common.constant_mixin import ConstantMixin
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.tflite2.common import LOG, TFLiteImportException
from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.tflite2.tflite_schema_head.ConcatenationOptions import \
    ConcatenationOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph
@tflite_op("CONCATENATION")
class Concatenation(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(ConcatenationOptions)
        G = kwargs['G']
        all_nodes = kwargs['all_nodes']
        opts = kwargs['opts']

        inputs = [all_nodes[t] for t in node.input]
        inp_shapes = [list(input[2].shape) for input in inputs]
        if not all(len(shape) == len(inp_shapes[0]) for shape in inp_shapes[1:]):
            raise TFLiteImportException(
                f"Concat importer: {node.name} doesn't have all the shapes of same lenght {inp_shapes}")

        buffer_idxes = [tensor.buffer_idx for tensor in node.input]
        non_zero_idxes = [idx for idx in buffer_idxes if idx != 0]
        duplicates = [idx for idx, count in Counter(
            non_zero_idxes).items() if count > 1]
        if duplicates:
            LOG.warning(
                f'concat {node.name} has duplicate inputs. Inserting copies but this is not very efficient.')
            for idx in duplicates:
                dup_idxes = [i for i, x in enumerate(buffer_idxes) if x == idx]
                for dup_idx in dup_idxes[1:]:
                    cparams = CopyNode(G.unique_name(
                        f'{node.name}_dup_{dup_idxes[0]}_{dup_idx}'))
                    dup_inp = inputs[dup_idx]
                    G.add_edge(
                        NNEdge(from_node=dup_inp[0], from_idx=dup_inp[1], to_node=cparams))
                    inputs[dup_idx] = tuple([cparams, 0] + list(dup_inp[2:]))

        concat_axis = node_opts.Axis()
        if concat_axis < 0:
            concat_axis += len(inp_shapes[0])

        # Notes:
        # some inputs may have defined axis which other inputs do not
        # some inputs may not be defined on concat axis
        # insert reshapes before inputs with undefined axes if cannot reduce to a constant

        # check for all defined axis
        defined_axes = [set([idx for idx, dim in enumerate(shape) if dim is not None])
                        for shape in inp_shapes]
        # the axes that must be defined are all in the input + the axis on
        # which the concat occurs
        all_defined_axes = set.union(*defined_axes) | {concat_axis}

        # make sure that all axes in the output are defined
        # if they are not set to 1
        adjusted_inp_shapes = []
        for idx, inp_shape in enumerate(inp_shapes):
            if defined_axes[idx] != all_defined_axes:
                inp_shape = [1 if idx in all_defined_axes and dim is None else dim
                             for idx, dim in enumerate(inp_shape)]
            adjusted_inp_shapes.append(tuple(inp_shape))

        # now calculate the output shape validating that all dims
        # are the same on non concat axes
        out_shape = []
        for axis, _ in enumerate(adjusted_inp_shapes[0]):
            if axis == concat_axis:
                dim = sum(inp_shape[concat_axis]
                          for inp_shape in adjusted_inp_shapes)
            elif axis in all_defined_axes:
                dim = adjusted_inp_shapes[0][axis]
                if not all(dim == inp_shape[axis] for inp_shape in adjusted_inp_shapes[1:]):
                    raise TFLiteImportException(f'{node.name} concat has mismatched inputs on axis {axis}')
            else:
                dim = None
            out_shape.append(dim)

        if all(cls.is_constant(inp) for inp in inputs):
            # by definition all output axes are defined here
            LOG.info("reducing %s to a constant", node.name)
            value = np.concatenate([cls.get_constant(inp)
                                    for inp in inputs], axis=concat_axis)
            params = ConstantInputNode(node.name, value=value)
        else:
            # need to insert a reshape on any input where all_defined_axes != all_axes
            for inp_idx, inp_shape in enumerate(inp_shapes):
                if defined_axes[inp_idx] == all_defined_axes:
                    continue
                reshape = ReshapeNode(
                    G.unique_name(f'{node.name}_reshape_{inp_idx}'),
                    old_shape=inp_shape,
                    shape=adjusted_inp_shapes[inp_idx])
                inp = inputs[inp_idx]
                G.add_edge(NNEdge(from_node=inp[0], from_idx=inp[1], to_node=reshape))
                inputs[inp_idx] = (reshape, 0)

            # concat axis needs to be adjusted for undefined axes before it
            concat_axis -= sum(
                1 if dim is None else 0 for dim in out_shape[:concat_axis:])

            # create concat and attach inputs
            params = ConcatNode(node.name, axis=concat_axis)

            for idx, inp in enumerate(inputs):
                G.add_edge(NNEdge(from_node=inp[0], to_node=params,
                                  from_idx=inp[1], to_idx=idx))

        if opts.get('load_quantization'):
            G.quantization[params.name] = cls.load_tf_quantization(
                node.input, node.output)
        cls.fuse_activation(node_opts, node.name, params, **kwargs)
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(out_shape))
        return params

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
