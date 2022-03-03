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
from graph.types import ConstantInputParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim

from ..backend_handler import BackendHandler
from ..handler import constant_only, onnx_op


@onnx_op("Range")
@constant_only(True)
class Range(BackendHandler, ConstantMixin):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        value = node.attrs.get('value', 0)
        inputs = [all_nodes[inp] if inp else None for inp in node.input]
        if len(inputs) != 3:
            raise ValueError(f'Range {valid_name} does not have 3 inputs')
        start, limit, delta = [cls.get_constant(x) for x in inputs]
        value = np.arange(start, limit, delta, dtype=start.dtype)
        params = ConstantInputParameters(valid_name,
                                         value=value)
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(value.shape), None)
        return params

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)
