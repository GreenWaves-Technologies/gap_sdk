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
from graph.types import LeakyActivationParameters

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from .math_mixin import BasicMathMixin


@onnx_op("LeakyRelu")
class Atan(BasicMathMixin, BackendHandler):
    @classmethod
    def _common(cls, node, **kwargs):
        alpha = node.attrs.get('alpha', 0.01)
        return super(Atan, cls)._common(node,
                                        params_class=LeakyActivationParameters,
                                        params_args={
                                            'leak_factor': alpha
                                        },
                                        constant_operation=(lambda y: lambda x: np.where(x > 0, x, x * y))(alpha),
                                        **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)
