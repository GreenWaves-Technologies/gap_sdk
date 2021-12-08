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

import gsystree as st
from gap.gap9.gap9_evk import Gap9_evk
from devices.gap_testbench.gap_testbench import Gap_testbench


class Gap9_evk_with_testbench(st.Component):

    def __init__(self, parent, name):
        super(Gap9_evk_with_testbench, self).__init__(parent, name)

        gap = Gap9_evk(self, 'gap')
        testbench = Gap_testbench(self, 'testbench')

        self.bind(testbench, 'i2s0', gap, 'i2s0')
        self.bind(testbench, 'i2s1', gap, 'i2s1')
        self.bind(testbench, 'i2s2', gap, 'i2s2')