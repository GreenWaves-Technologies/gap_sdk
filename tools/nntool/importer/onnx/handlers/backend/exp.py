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
from graph.types.others import UnaryOpParameters

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from .math_mixin import BasicMathMixin


@onnx_op("Exp")
class Exp(BasicMathMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        return super(Exp, cls)._common(node,
                                       params_class=UnaryOpParameters,
                                       params_args={'op_type': 'exp'},
                                       constant_operation=np.exp,
                                       **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)
