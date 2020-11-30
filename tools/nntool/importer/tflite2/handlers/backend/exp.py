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

from graph.dim import Dim
from graph.types import UnconvertedOpParameters

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .math_mixin import BasicMathMixin


@tflite_op("EXP")
class Exp(BasicMathMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[t] for t in node.input]
        x = inputs[0]
        return super(Exp, cls)._common(
            node,
            params_class=UnconvertedOpParameters,
            params_args={
                'indicated_op_name': 'EXP',
                'indicated_outputs': [Dim.unnamed(x[2].known_shape)]
            },
            **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
