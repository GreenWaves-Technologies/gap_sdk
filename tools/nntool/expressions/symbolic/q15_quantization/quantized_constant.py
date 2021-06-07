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

from ..symbol import Constant, Symbol


class QuantizedConstant(Constant):
    def __init__(self, *args, dtype=np.int32, **kwargs):
        super().__init__(*args, dtype=dtype, **kwargs)

class QuantizedValue(Symbol):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def _calculate(self, calculate_ranges=False, **kwargs):
        raise ValueError('wrapper class for quantization purposes - not designed to be evaluated')

    def _impl(self, *args, **kwargs):
        raise ValueError('wrapper class for quantization purposes - not designed to be evaluated')
