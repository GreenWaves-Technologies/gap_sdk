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


class Gap_testbench(st.Component):

    def __init__(self, parent, name):
        super(Gap_testbench, self).__init__(parent, name)

        gap = Gap9_evk(self, 'gap0')

        firmware = 'devices/gap_testbench/gap_firmware.img'
        firmware_img = self.get_file_path(firmware)

        if firmware_img is None:
            raise RuntimeError('Could not find gap firmware: ' + firmware)

        self.add_property('gap0/flash/content/image', firmware_img)

        self.bind(gap, 'i2s0', self, 'i2s0')
        self.bind(gap, 'i2s1', self, 'i2s1')
        self.bind(gap, 'i2s2', self, 'i2s2')

