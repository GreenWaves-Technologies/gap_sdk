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
from nntool.graph.types.tensor_arithmetic import MatrixMulNode, MatrixSubNode, SquaredDifferenceNode
from nntool.importer.tflite2.tflite_schema_head.SquaredDifferenceOptions import \
    SquaredDifferenceOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .math_mixin import ArithmeticMixin

def insert_node(G, name, inputs):
    sub1 = MatrixSubNode(G.unique_name(f'{name}_sub1'))
    sub2 = MatrixSubNode(G.unique_name(f'{name}_sub2'))
    mul1 = MatrixMulNode(G.unique_name(f'{name}_mul'))
    links = [
        {
            'from_node': inputs[0][0],
            'from_idx': inputs[0][1],
            'to_node': sub1,
            'to_idx': 0,
        },
        {
            'from_node': inputs[1][0],
            'from_idx': inputs[1][1],
            'to_node': sub1,
            'to_idx': 1,
        },
        {
            'from_node': inputs[0][0],
            'from_idx': inputs[0][1],
            'to_node': sub2,
            'to_idx': 1,
        },
        {
            'from_node': inputs[1][0],
            'from_idx': inputs[1][1],
            'to_node': sub2,
            'to_idx': 0,
        },
        {
            'from_node': sub1,
            'from_idx': 0,
            'to_node': mul1,
            'to_idx': 0,
        },
        {
            'from_node': sub2,
            'from_idx': 0,
            'to_node': mul1,
            'to_idx': 1,
        },
    ]
    return (mul1, links)

@tflite_op("SQUARED_DIFFERENCE")
class SquaredDifference(ArithmeticMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        node_opts = node.get_options(SquaredDifferenceOptions)
        kwargs['node_opts'] = node_opts
        return super(SquaredDifference, cls)._common(node,
                                       params_class=insert_node,
                                       constant_operation=lambda x, y: np.square(x - y),
                                       **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
