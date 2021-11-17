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

from importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler
from ..handler import tflite_op

from .pad_mixin import PadMixin


@tflite_op("PADV2")
class PadV2(PadMixin, BackendHandler):
# xinglong debug use pad.py code here. can compile uint8 yolov3 tflite
    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):

        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[t] for t in node.input]
        pads = list(cls._verify_constant(inputs[1]))
        node.input[1].used = True
        return super(PadV2, cls)._common(node, pads=pads, **kwargs)

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
