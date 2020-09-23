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

import runner.rtl.rtl_runner
import runner.chips.pulp
import os


class Runner(runner.rtl.rtl_runner.Runner, runner.chips.pulp.Runner):

    def __init__(self, args, config):
        runner.rtl.rtl_runner.Runner.__init__(self, args, config)
        runner.chips.pulp.Runner.__init__(self, args, config)

        self.__process_args()

        self.set_arg('-gHYPER_FLASH_LOAD_MEM=1')
        self.set_arg('+VSIM_PADMUX_CFG=TB_PADMUX_ALT3_HYPERBUS')
        self.set_arg('+VSIM_BOOTTYPE_CFG=TB_BOOT_FROM_HYPER_FLASH')

        if os.environ.get('QUESTA_CXX') is not None:
            self.set_arg('-dpicpppath ' + os.environ.get('QUESTA_CXX'))

        self.set_arg('-permit_unmatched_virtual_intf')
        self.set_arg('+preload_file=efuse_preload.data')
        self.set_arg('-gBAUDRATE=115200')

        self.set_env('VOPT_ACC_ENA', 'YES')

        path = os.path.join(self.config.get_str('gapy/work_dir'), 'efuse_preload.data')



    def dump(self, str):
        if self.verbose:
            print (str)
