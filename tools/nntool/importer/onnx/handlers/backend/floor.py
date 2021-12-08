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
from graph.types import ConstantInputParameters
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import onnx_op, ps_description, partial_support


@onnx_op("Floor")
@partial_support(True)
@ps_description("only implemented to eliminate from contants. no backend kernel in nntool currently.")
class Floor(ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape

        if cls.is_constant(x):
            x_val = cls.get_constant(x)
            res = np.floor(x_val)
            if x_val.size < 10:
                logger.info("reducing %s to a constant %s", valid_name, x_val)
            else:
                logger.info("reducing %s to a constant", valid_name)
            params = ConstantInputParameters(valid_name, dims=Dim.unnamed(x_val.shape), value=res)
        else:
            raise ValueError("ONNX floor operator is not implemented")
        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(x_shape), None)
        return params

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
