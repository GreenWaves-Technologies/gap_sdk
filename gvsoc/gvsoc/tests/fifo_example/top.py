#
# Copyright (C) 2018 ETH Zurich and University of Bologna
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 
import vp_core as vp

class component(vp.component):

    def build(self):

        clock = self.new('clock', component='vp/clock_domain', config=self.get_config().get_config('clock_domain'))

        master = self.new('master', component='master', config=self.get_config('master'))

        slave = self.new('slave', component='slave', config=self.get_config('slave'))

        master.get_port('out').bind_to(slave.get_port('in_0'))

        clock.get_port('out').bind_to(master.get_port('clock'))
        clock.get_port('out').bind_to(slave.get_port('clock'))
