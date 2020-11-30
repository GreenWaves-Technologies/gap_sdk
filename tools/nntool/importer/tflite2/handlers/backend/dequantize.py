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
from ..handler import tflite_op, partial_support, ps_description

from .no_op_mixin import NoOpMixin


@tflite_op("DEQUANTIZE")
@partial_support(True)
@ps_description("imported but not used in calculations")
class Dequantize(NoOpMixin, BackendHandler):
    pass
