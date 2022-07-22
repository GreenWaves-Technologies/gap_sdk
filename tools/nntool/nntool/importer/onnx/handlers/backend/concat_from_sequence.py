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
from nntool.graph.types import ConstantInputNode, NNEdge, ReshapeNode
from nntool.importer.common.provisional_dim import ProvisionalDim
from nntool.importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from .concat_mixin import ConcatMixin


@onnx_op("ConcatFromSequence")
class ConcatFromSequence(ConcatMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        input_shapes = [inp[2].shape for inp in inputs]
        axis = node.attrs['axis']
        new_axis = node.attrs.get('new_axis', 0)
        # if new_axis is false this is the same as concat
        if not new_axis:
            return cls.gen_concat(node, inputs, axis)
        # if it is true then it's different
        if not all(shape == input_shapes[0] for shape in input_shapes[1::]):
            raise ValueError('all shapes must be the same in ConcatFromSequence with new axis')

        # reduce to a constant if we can
        if all(cls.is_constant(inp) for inp in inputs):
            logger.info("reducing %s to a constant", valid_name)
            value = np.concatenate([cls.get_constant(inp) for inp in inputs], axis=axis)
            params = ConstantInputNode(valid_name, value=value)
            all_nodes[node.output[0]] = (params, 0, ProvisionalDim(value.shape), inputs[0][3])
            return params

        # add the axis into the shape
        new_shape = input_shapes[0].copy()
        new_shape = new_shape[:axis:] + [1] + new_shape[axis::]
        old_shape = cls._get_real_dim(input_shapes[0])
        shape = cls._get_real_dim(new_shape)
        # create a reshape on each input and pass the outputs to the concat mixin
#pylint: disable=consider-using-enumerate
        for idx in range(len(inputs)):
            inp = inputs[idx]
            rparams = ReshapeNode(
                "%s_reshape_%s" % (valid_name, idx),
                old_shape=old_shape,
                shape=shape)
            G.add_edge(NNEdge(from_node=inp[0], to_node=rparams, from_idx=inp[1], to_idx=0))
            inputs[idx] = (rparams, 0, ProvisionalDim(new_shape), inp[3])

        return cls.gen_concat(node, inputs, axis)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)
