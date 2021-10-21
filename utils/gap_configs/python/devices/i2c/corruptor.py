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


class Corruptor(st.Component):

    def __init__(self, parent, name, address=80):
        super(Corruptor, self).__init__(parent, name)

        corruptor = Corruptor.Corruptor_implem(self, 'corruptor', address=address)

        clock = Clock_domain(self, 'clock', frequency=10000000)

        self.bind(clock, 'out', corruptor, 'clock')
        self.bind(corruptor, 'i2c', self, 'i2c')
        self.bind(corruptor, 'clock_cfg', clock, 'clock_in')


    class Corruptor_implem(st.Component):

        def __init__(self, parent, name, address):
            super(Corruptor.Corruptor_implem, self).__init__(parent, name)
            self.add_property('vp_component', 'devices.i2c.corruptor.i2c_corruptor')
