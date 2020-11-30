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

from ..backend_handler import BackendHandler
from ..handler import onnx_op
from ..handler import partial_support
from ..handler import ps_description
from .pool_mixin import PoolMixin


@onnx_op("MaxPool")
@partial_support(True)
@ps_description("only support maxpool with 2d input")
class MaxPool(PoolMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        if len(node.output) > 1:
            ValueError("max pool with argmax is not supported")
        return cls.pool(node, pool_type="max", **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_8(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_10(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls._common(node, **kwargs)

    @classmethod
    def version_12(cls, node, **kwargs):
        return cls._common(node, **kwargs)
