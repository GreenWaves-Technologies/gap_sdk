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

from graph.dim import PadDim
from importer.tflite2.tflite_schema_head.Padding import Padding

class FilterPadMixin(object):
    @classmethod
    def get_tf_padding(cls, padding):
        if padding == Padding.SAME:
            return PadDim.same()
        if padding == Padding.VALID:
            return PadDim.valid()
        raise ValueError("Strange padding type")
