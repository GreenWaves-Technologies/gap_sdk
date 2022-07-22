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

from nntool.importer.tflite2.common.tflite_node import TFLiteNode

from ..backend_handler import BackendHandler
from ..handler import tflite_op, ps_description, partial_support

from .pad_mixin import PadMixin


@tflite_op("PADV2")
@partial_support(True)
@ps_description('Padding is only supported on GAP when it can be fused into another layer that supports it like '
                'a pooling or convolution layer.')
class PadV2(PadMixin, BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):

        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[t] for t in node.input]
        pads = list(cls._verify_constant(inputs[1]))
        node.input[1].used = True
        pad_vals = list((v, v) for v in cls._verify_constant(inputs[2]))
        node.input[2].used = True
        return super(PadV2, cls)._common(node, pads=pads, pad_vals=pad_vals, **kwargs)

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
