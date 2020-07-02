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

import gv.gvsoc
import runner.chips.pulp
import os



class Runner(gv.gvsoc.Runner, runner.chips.pulp.Runner):

    def __init__(self, args, config):
        gv.gvsoc.Runner.__init__(self, args, config)
        runner.chips.pulp.Runner.__init__(self, args, config)

    def gen_stimuli(self):
        gv.gvsoc.Runner.gen_stimuli(self)
