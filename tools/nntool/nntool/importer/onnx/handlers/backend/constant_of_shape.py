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
from nntool.graph.dim import Dim
from nntool.graph.types import ConstantInputNode
from nntool.importer.common.provisional_dim import ProvisionalDim
from onnx import numpy_helper

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("ConstantOfShape")
class ConstantOfShape(BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        value = node.attrs.get('value', 0)
        inputs = [all_nodes[inp] if inp else None for inp in node.input]
        x = inputs[0]
        const_shape = list(x[0].value)
        params = ConstantInputNode(valid_name,
                                         dims=Dim.unnamed(const_shape),
                                         value=np.full(const_shape, numpy_helper.to_array(value)))
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(const_shape), None)
        return params

    @classmethod
    def version_9(cls, node, **kwargs):
        return cls._common(node, **kwargs)
