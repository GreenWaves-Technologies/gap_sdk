#
# Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
#                    University of Bologna
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
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#


import gv.gvsoc
import runner.chips.gap9_v2
import os
from elftools.elf.elffile import *



class Runner(gv.gvsoc.Runner, runner.chips.gap9_v2.Runner):

    def __init__(self, args, config):

        # Check if the binary contains special riscv fesvr tohost symbol for interations
        # between simulated code and testbench
        with open(args.binary, 'rb') as file:
            elffile = ELFFile(file)
            for section in elffile.iter_sections():
                if isinstance(section, SymbolTableSection):
                    for symbol in section.iter_symbols():
                        if symbol.name == 'tohost':
                            config.get('**/soc/fc').set('riscv_fesvr_tohost_addr', '0x%x' % symbol.entry['st_value'])

        gv.gvsoc.Runner.__init__(self, args, config)
        runner.chips.gap9_v2.Runner.__init__(self, args, config)



    def gen_stimuli(self):
        gv.gvsoc.Runner.gen_stimuli(self)

        path = os.path.join(self.config.get_str('gapy/work_dir'), 'efuse_preload.data')
        self.gen_efuse_stim(path)
