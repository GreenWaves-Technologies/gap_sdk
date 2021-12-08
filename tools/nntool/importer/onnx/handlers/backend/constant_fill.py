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
from importer.common.provisional_dim import ProvisionalDim
from graph.types import ConstantInputParameters

from ..backend_handler import BackendHandler
from ..handler import onnx_op, domain


@onnx_op("ConstantFill")
@domain("__extensions")
class ConstantFill(BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        value = node.attrs['value']
        if node.attrs.get('input_as_shape'):
            shape = all_nodes[node.input[0]][0].value
        else:
            shape = node.attrs.get('shape')
        value = np.full(shape, value)
        params = ConstantInputParameters(valid_name, dims=Dim.unnamed(value.shape), value=value)
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(value.shape), None)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
