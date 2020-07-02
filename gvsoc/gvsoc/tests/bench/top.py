#
# Copyright (C) 2020  GreenWaves Technologies, SAS
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

import vp_core as vp

class component(vp.component):

    def build(self):

        clock = self.new('clock', component='vp/clock_domain', config=self.get_config().get_config('clock_domain'))

        master = self.new('master', component='master', config=self.get_config())

        slave = self.new('slave', component='slave', config=self.get_config())

        master.get_port('out').bind_to(slave.get_port('in'))

        clock.get_port('out').bind_to(master.get_port('clock'))
