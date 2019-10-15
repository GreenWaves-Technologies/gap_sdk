#
# Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

# 
# Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
#

import runner.rtl.vsim_runner as runner

# This class is the default runner for all chips but can be overloaded
# for specific chips in order to change a bit the behavior.
class Runner(runner.Runner):

    def __init__(self, config, js_config):

        super(Runner, self).__init__(config, js_config)

        # All dev modes should set padsel pad to 1 so that boot code knows
        # it should stop and the external loader take over the boot through JTAG
        boot = self.get_json().get_child_str('**/runner/boot-mode')
        if boot.find('_dev') != -1:
            self.set_param('VSIM_BOOTMODE_CFG', 'TB_BOOT_MODE_1')

        if boot.find('_mram') != -1:
            self.set_param('PRELOAD_MRAM', '1')
            self.set_param('REF_LINE_INIT', '1')