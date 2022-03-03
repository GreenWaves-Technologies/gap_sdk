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
from graph.types import ConstantInputParameters, ExpandParameters
from graph.types.base import NNEdge
from importer.common.constant_mixin import ConstantMixin
from importer.common.provisional_dim import ProvisionalDim
from importer.onnx.common import logger

from ..backend_handler import BackendHandler
from ..handler import constant_only, onnx_op
from .broadcast_mixin import BroadcastMixin


@onnx_op("Expand")
@constant_only(True)
class Expand(BroadcastMixin, ConstantMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        valid_name = kwargs['valid_name']
        G = kwargs['G']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        y = inputs[1]
        shape = cls.get_constant(y)

        if cls.is_constant(x):
            logger.info("reducing %s to a constant", valid_name)
            x_val = cls.get_constant(x)
            params = ConstantInputParameters(valid_name, value=x_val * np.ones(shape))
        else:
            params = ExpandParameters(valid_name, shape=shape)
            G.add_edge(NNEdge(x[0], params, from_idx=x[1]))

        all_nodes[node.output[0]] = (params, 0, ProvisionalDim(shape), x[3])
        return params

    @classmethod
    def version_8(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
