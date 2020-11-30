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

from graph.types import NearestNeighborResizerParameters
from importer.tflite2.common.tflite_node import TFLiteNode
from importer.tflite2.tflite_schema_head.ResizeNearestNeighborOptions import \
    ResizeNearestNeighborOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .resize_mixin import ResizeMixin


@tflite_op("RESIZE_NEAREST_NEIGHBOR")
class ResizeNearestNeighbour(ResizeMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        return super(ResizeNearestNeighbour, cls)._common(
            node,
            params_class=NearestNeighborResizerParameters,
            opts_class=ResizeNearestNeighborOptions,
            **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
