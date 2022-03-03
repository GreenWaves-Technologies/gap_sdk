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
from graph.types import ConstantInputParameters, NNEdge
from graph.types.others import ScatterNdParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description

def scatter_nd_impl(data, indices, updates, reduction='none'):
    # Check tensor shapes
    assert indices.shape[-1] <= len(data.shape)
    assert updates.shape == indices.shape[:-1] + data.shape[indices.shape[-1]:]

    # Compute output
    output = np.copy(data)
    for i in np.ndindex(indices.shape[:-1]):
        if reduction == 'add':
            output[indices[i]] += updates[i]
        elif reduction == 'mul':
            output[indices[i]] *= updates[i]
        else:
            output[indices[i]] = updates[i]
    return output

@onnx_op("ScatterND")
@partial_support(True)
@ps_description('ScatterND is only supported at input and is not supported by nntool or autotiler kernels')
class ScatterND(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        indices = cls.get_constant(inputs[1])
        updates = inputs[2]
        reduction = node.attrs.get('reduction', None)

        pshape = ProvisionalDim(x_shape)
        if cls.is_constant(x) and cls.is_constant(updates):
            logger.info("reducing %s to a constant", valid_name)
            x_val = cls.get_constant(x)
            updates_val = cls.get_constant(updates)
            params = ConstantInputParameters(valid_name, value=scatter_nd_impl(x_val, indices, updates_val, reduction=reduction))
        else:
            logger.warning(f'{valid_name} ScatterND is not currently supported in the nntool or Autotiler kernels')
            params = ScatterNdParameters(valid_name, indices=indices, updates=updates, reduction=reduction)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
            G.add_edge(NNEdge(from_node=updates[0], to_node=params, from_idx=updates[1], to_idx=1))
        all_nodes[node.output[0]] = (params, 0, pshape, x[3])
        return params

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_16(cls, node, **kwargs):
        return cls._common(node, **kwargs)
