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

from nntool.graph.dim import Dim
from nntool.graph.types import UnconvertedOpNode
from nntool.importer.tflite2.handlers.backend.math_mixin import BasicMathMixin

class UnconvertedMixin(BasicMathMixin):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        return super(UnconvertedMixin, cls)._common(
            node,
            params_class=UnconvertedOpNode,
            params_args={
                'indicated_op_name': cls.TFLITE_OP,
                'indicated_outputs': [Dim.unnamed(x[2].known_shape)]
            },
            **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
