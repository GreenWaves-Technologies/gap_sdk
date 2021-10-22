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
from graph.types.tensor_arithmetic import SquaredDifferenceParameters
from importer.tflite2.tflite_schema_head.SquaredDifferenceOptions import \
    SquaredDifferenceOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .math_mixin import ArithmeticMixin


@tflite_op("SQUARED_DIFFERENCE")
class SquaredDifference(ArithmeticMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        node_opts = node.get_options(SquaredDifferenceOptions)
        kwargs['node_opts'] = node_opts
        return super(SquaredDifference, cls)._common(node,
                                       params_class=SquaredDifferenceParameters,
                                       constant_operation=lambda x, y: np.square(x - y),
                                       **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
