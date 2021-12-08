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


from importer.onnx.handlers.backend.mat_mul_mixin import MatMulMixin
from importer.onnx.handlers.backend_handler import BackendHandler

from ..handler import onnx_op, partial_support, ps_description


@onnx_op("QLinearMatMul")
@partial_support(True)
@ps_description("Supports only 2D or groups of 2D matrix multiplications.")
class QLinearMatMul(BackendHandler, MatMulMixin):
    @classmethod
    def _common(cls, node, **kwargs):
        return cls._handle(node, quantized=True, **kwargs)

    @classmethod
    def version_10(cls, node, **kwargs):
        return cls._common(node, **kwargs)
