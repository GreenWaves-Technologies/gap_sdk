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

# pylint: disable=no-name-in-module

import numpy as np

from .float_quantization_base import (
    FloatFilterQuantizationRecordBase, FloatQuantizationRecordBase,
    FloatScalableFilterQuantizationRecordBase)


class Float16QuantizationRecord(FloatQuantizationRecordBase):
    TYPE = "IEEE16"
    DTYPE = np.float16

class Float16FilterQuantizationRecord(FloatFilterQuantizationRecordBase):
    TYPE = "IEEE16"
    DTYPE = np.float16

class Float16ScalableFilterQuantizationRecord(FloatScalableFilterQuantizationRecordBase):
    TYPE = "IEEE16"
    DTYPE = np.float16
