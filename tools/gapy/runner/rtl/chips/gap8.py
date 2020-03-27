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

import runner.rtl_runner
import os


class Runner(runner.rtl_runner.Runner):

    def __init__(self, args, config):
        super(Runner, self).__init__(args, config)
        self.__process_args()

        self.set_arg('-gHYPER_FLASH_LOAD_MEM=1')
        self.set_arg('+VSIM_PADMUX_CFG=TB_PADMUX_ALT3_HYPERBUS')
        self.set_arg('+VSIM_BOOTTYPE_CFG=TB_BOOT_FROM_HYPER_FLASH')

        if os.environ.get('QUESTA_CXX') is not None:
            self.set_arg('-dpicpppath ' + os.environ.get('QUESTA_CXX'))

        self.set_arg('-permit_unmatched_virtual_intf')
        self.set_arg('+preload_file=efuse_preload.data')
        self.set_arg('-gBAUDRATE=115200')

        path = os.path.join(self.config.get_str('gapy/work_dir'), 'efuse_preload.data')
        self.gen_efuse_stim(path)


    def gen_efuse_stim(self, filename):
        self.dump('Creating efuse stimuli')

        nb_regs = 128
        efuses = [0] * nb_regs

        efuses[0] = 0x2a

        values = [0] * nb_regs * 8
        for efuse_id in range(0, nb_regs):
            value = efuses[efuse_id]
            self.dump('  Writing register (index: %d, value: 0x%x)' % (efuse_id, value))
            for index in range(0, 8):
                if (value >> index) & 1 == 1:
                    values[efuse_id + index*128] = 1

        self.dump('  Generating to file: ' + filename)

        with open(filename, 'w') as file:
            for value in values:
                file.write('%d ' % (value))


    def dump(self, str):
        if self.verbose:
            print (str)
