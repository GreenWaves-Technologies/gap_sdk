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

import traces

class Runner(object):

    def __init__(self, args, config):
        pass

    def gen_efuse_stim(self, filename):
        traces.info('Creating efuse stimuli')

        nb_regs = 128
        efuses = [0] * nb_regs

        chip = self.config.get_str('**/chip')

        if chip == 'gap8_revc':

            if self.get_boot_mode() == 'flash':
                efuses[0] = 2 | (2 << 3) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7)

                if self.get_boot_flash_type() == 'spi':
                    # SPI flash type
                    efuses[37] = (0 << 0)

                else:
                    # Hyperflash type
                    efuses[37] = (1 << 0)


        elif chip == 'gap_rev1':

            if self.get_boot_mode() == 'flash':
                efuses[0] = 2 | (2 << 3) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7)

                if self.get_boot_flash_type() == 'spi':
                    # SPI flash type
                    efuses[37] = (0 << 0)

                else:
                    # Hyperflash type
                    efuses[37] = (1 << 0)

        else:

            # RTL platform | flash boot | no encryption | no wait xtal
            efuses[0] = 0x2A


        values = [0] * nb_regs * 8
        for efuseId in range (0, nb_regs):
            value = efuses[efuseId]
            traces.info('  Writing register (index: %d, value: 0x%x)' % (efuseId, value))
            for index in range(0, 8):
                if (value >> index) & 1 == 1: values[efuseId + index*128] = 1

        traces.info('  Generating to file: ' + filename)

        with open(filename, 'w') as file:
            for value in values:
                file.write('%d ' % (value))
