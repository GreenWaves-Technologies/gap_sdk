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

from importer.common.constant_mixin import ConstantMixin
from quantization.qtype import QType

from ..backend_handler import BackendHandler
from ..handler import onnx_op


@onnx_op("DequantizeLinear")
class DequantizeLinear(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[inp] for inp in node.input]
        axis = node.attrs.get('axis', None)
        x = inputs[0]
        scale = inputs[1]
        zero_point = inputs[2] if len(inputs) > 2 else None
        qtype = QType(
            dtype=zero_point[0].value.dtype, scale=scale[0].value,
            zero_point=zero_point[0].value, quantized_dimension=axis)
        all_nodes[node.input[0]] = tuple(list(x)[0:3] + [qtype])
        all_nodes[node.output[0]] = tuple(list(x)[0:3] + [None])
        return x[0]

    @classmethod
    def version_10(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
