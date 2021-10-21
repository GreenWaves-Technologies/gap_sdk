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

import runner.chips.gap9_v2
import runner.rtl.rtl_runner
import os
import os.path
from elftools.elf.elffile import ELFFile


class Runner(runner.chips.gap9_v2.Runner, runner.rtl.rtl_runner.Runner):

    def __init__(self, args, config, system):
        runner.chips.gap9_v2.Runner.__init__(self, args, config, system)
        runner.rtl.rtl_runner.Runner.__init__(self, args, config, system)

        #self.set_arg('-gSPI_FLASH_LOAD_MEM=3')
        jtag_mode = self.config.get_str('**/runner/jtag_mode')
        if jtag_mode is not None:
            self.set_cmd_arg('+JTAG_MODE=%s' % jtag_mode.upper())

        ocd_port = self.config.get_int('**/runner/openocd_port')
        if ocd_port is None:
            ocd_port = os.environ.get('GAPY_OPENOCD_BITBANG_PORT')
            if ocd_port is not None:
                ocd_port = int(ocd_port)

        if ocd_port is not None:
            self.set_cmd_arg('+OPENOCD_PORT=%d' % ocd_port)

        #self.set_cmd_arg('+CONFIG_I2S_TRACE=1')

        with open(self.config.get_str('runner/boot-loader'), 'rb') as file:
            entry = ELFFile(file)['e_entry']

        self.set_cmd_arg('+TB_DEBUG_VERBOSITY=%d' % self.config.get_int('**/rtl/verbosity'))

        boot_mode = self.config.get_str('**/runner/boot/mode')
        if boot_mode == 'jtag' or self.config.get_bool('**/runner/boot/jtag_force'):
            self.set_cmd_arg('+VSIM_BOOTMODE_CFG=1')
            self.set_cmd_arg('+EXEC_TEST=ROM_JTAG_BOOT')
            self.set_cmd_arg('+EXEC_TEST_BOOT_ADDRESS=%x' % entry)
        elif boot_mode == 'spislave':
            self.full_config.set('**/system_tree/board/testbench/testbench/spislave_boot/stim_file', 'slm_files/l2_stim.slm')
            self.full_config.set('**/system_tree/board/testbench/testbench/spislave_boot/entry', '0x%x' % entry)
            self.full_config.set('**/runner/gvsoc_dpi/enabled', True)

        jtag_mode = self.config.get_int('**/runner/boot/jtag_mode')
        if jtag_mode is not None:
            self.set_cmd_arg('+VSIM_BOOTMODE_CFG=%d' % jtag_mode)

        self.set_arg('-gHYPER_FLASH_LOAD_MEM=1')

        path = os.path.join(self.config.get_str('gapy/work_dir'), 'slm_files/mram_stim.slm')
        if os.path.exists(path):
            self.set_cmd_arg('+PRELOAD_MRAM=1')

        if self.platform_tool == 'vsim':
            path = '%s/ips_inputs/dpi/librbs' % self.__get_platform_path()
        else:
            path = '%s/ips_inputs/dpi/librbs.so' % self.__get_platform_path()

        if os.path.exists(path):
            self.set_cmd_arg('-sv_lib %s' % path)
        #self.set_arg('-gCONFIG_FILE=rtl_config.json')
        #self.set_arg('-sv_lib %s' % (os.path.join(os.environ.get('INSTALL_DIR'), 'lib', 'libpulpdpi')))

        if os.environ.get('QUESTA_CXX') is not None:
            self.set_arg('-dpicpppath ' + os.environ.get('QUESTA_CXX'))

        self.set_arg('-permit_unmatched_virtual_intf')
        self.set_arg('+preload_file=efuse_preload.data')

        uart_baudrate = self.config.get_int('**/rtl/testbench/uart/baudrate')
        if uart_baudrate is not None:
            self.set_cmd_arg('+CONFIG_UART_BAUDRATE=%d' % uart_baudrate)

        self.set_env('VOPT_ACC_ENA', 'YES')




    def flash(self):
        path = os.path.join(self.config.get_str('gapy/work_dir'), 'efuse_preload.data')
        self.gen_efuse_stim(path)
        return runner.rtl.rtl_runner.Runner.flash(self)
