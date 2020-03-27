#
# Copyright (C) 2019 GreenWaves Technologies
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

import gv.gvsoc
import runner.chips.gap8
import os.path


class Runner(gv.gvsoc.Runner, runner.chips.gap8.Runner):

    def __init__(self, args, config):
        gv.gvsoc.Runner.__init__(self, args, config)
        runner.chips.gap8.Runner.__init__(self, args, config)


    def gen_stimuli(self):
        gv.gvsoc.Runner.gen_stimuli(self)

        path = os.path.join(self.config.get_str('gapy/work_dir'), 'efuse_preload.data')
        self.gen_efuse_stim(path)
