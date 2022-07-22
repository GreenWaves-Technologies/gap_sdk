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

from nntool.graph.types.tensor_arithmetic import MatrixAddNode

from ..backend_handler import BackendHandler
from ..handler import onnx_op, domain
from .math_mixin import ArithmeticMixin


@onnx_op("QLinearAdd")
@domain("com.microsoft")
class MsQLinearAdd(ArithmeticMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        return super(MsQLinearAdd, cls)._common(node,
                                                params_class=MatrixAddNode,
                                                constant_operation=lambda x, y: x + y,
                                                quantized_args=(
                                                    (0, 1, 2), (3, 4, 5), (6, 7)),
                                                **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
