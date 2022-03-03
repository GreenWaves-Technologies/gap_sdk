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

from graph.types.tensor_arithmetic import MatrixAddParameters

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from .math_mixin import ArithmeticMixin


@onnx_op("Add")
class Add(ArithmeticMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        return super(Add, cls)._common(node,
                                       params_class=MatrixAddParameters,
                                       constant_operation=lambda x, y: x + y,
                                       **kwargs)

    @classmethod
    def version_7(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_13(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_14(cls, node, **kwargs):
        return cls._common(node, **kwargs)
