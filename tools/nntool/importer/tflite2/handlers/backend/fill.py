# Copyright (C) 2021  GreenWaves Technologies, SAS

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
from importer.common.provisional_dim import ProvisionalDim
from importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler
from ..handler import tflite_op


@tflite_op("FILL")
class Fill(BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']

        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        if len(x_shape) != 1:
            raise ValueError(f'FILL {node.name} expecting 1D tensor for shape')

        shape = list(cls._verify_constant(inputs[0]))

        if cls._is_constant(inputs[1]):
            val = cls._get_constant(inputs[1])

            params = ConstantInputParameters(node.name,
                                            dims=Dim.unnamed(shape),
                                            value=np.full(shape, val),
                                            constant_store=G.constant_store)
            all_nodes[node.output[0]] = (params, 0, ProvisionalDim(shape))
            return params
        else:
            raise ValueError(f'FILL {node.name} non constant fill values are not currently supported')

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
