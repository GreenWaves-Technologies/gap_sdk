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

        info2 = 0

          # RTL platform | flash boot | no encryption | no wait xtal
        efuses[0] = 2 | (2 << 3) | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7)
        efuses[37] = 1 << 0   # Boot on UDMA hyper
        #efuses[39] = 0x2   # Boot on UDMA SPIM1 interface (first single spi)

        # Do not check bootmode pads by default to boot from what is specified in efuses
        info2 = (1 << 8) | (1 << 9)





        efuses[1] = info2


        traces.info('  Generating to file: ' + filename)

        with open(filename, 'w') as file:
            for efuseId in range (0, 128):
                value = efuses[efuseId]
                traces.info('  Writing register (index: %d, value: 0x%x)' % (efuseId, value))
                file.write('{0:032b}\n'.format(value))
  