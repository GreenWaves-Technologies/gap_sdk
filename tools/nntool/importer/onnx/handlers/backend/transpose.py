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


from graph.types import (ConstantInputParameters, NNEdge, NoOPParameters,
                         TransposeParameters)
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from importer.common.constant_mixin import ConstantMixin


@onnx_op("Transpose")
class Transpose(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        transpose = node.attrs.get('perm', list(range(len(x_shape) - 1, -1, -1)))
        transpose = tuple(transpose)
        pout_shape = [x_shape[i] for i in transpose]

        new_axes = {}
        for idx, dim in enumerate(x_shape):
            if dim is not None:
                new_axes[idx] = len(new_axes)
        if cls.is_constant(x):
            logger.info("reducing %s to a constant", valid_name)
            x_val = cls.get_constant(x)
            params = ConstantInputParameters(valid_name, value=x_val.transpose(transpose))
        else:
            transpose = [new_axes[axis] for axis in transpose if x_shape[axis] is not None]
            if transpose == sorted(transpose):
                params = NoOPParameters(valid_name, desc="transpose does nothing")
            else:
                params = TransposeParameters(valid_name, transpose=transpose)
            G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(pout_shape))
        return params


    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
