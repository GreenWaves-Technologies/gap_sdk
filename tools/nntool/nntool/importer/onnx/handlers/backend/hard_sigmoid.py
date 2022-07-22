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
from nntool.graph.types.activations import HSigmoidNode
from ..backend_handler import BackendHandler
from ..handler import onnx_op, partial_support, ps_description
from .math_mixin import BasicMathMixin


@onnx_op("HardSigmoid")
@ps_description("only supports alpha of 0.2 and beta of 0.2")
@partial_support(True)
class Sigmoid(BasicMathMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        alpha = node.attrs.get('alpha', 0.2)
        beta = node.attrs.get('beta', 0.5)
        params_args = {
            'offset': beta/alpha,
            'upper_bound': 1/alpha,
            'mult': alpha
        }
        return super(Sigmoid, cls)._common(node,
                                           params_class=HSigmoidNode,
                                           params_args=params_args,
                                           constant_operation=lambda x: np.maximum(0, np.minimum(1, alpha * x + beta)),
                                           **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls._common(node, **kwargs)
