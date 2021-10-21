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
from ips.clock.clock_domain import Clock_domain


class Eeprom(st.Component):

    def __init__(self, parent, name, address=80):
        super(Eeprom, self).__init__(parent, name)

        eeprom = Eeprom.Eeprom_implem(self, 'eeprom', address=address)

        clock = Clock_domain(self, 'clock', frequency=10000000)

        self.bind(clock, 'out', eeprom, 'clock')
        self.bind(eeprom, 'i2c', self, 'i2c')
        self.bind(eeprom, 'clock_cfg', clock, 'clock_in')


    class Eeprom_implem(st.Component):

        def __init__(self, parent, name, address):
            super(Eeprom.Eeprom_implem, self).__init__(parent, name)

            self.add_property('vp_component', 'devices.i2c.eeprom.i2c_eeprom')

            self.add_property('address', address)
            self.add_property('page_size', 16)
            self.add_property('number_of_pages', 256)
