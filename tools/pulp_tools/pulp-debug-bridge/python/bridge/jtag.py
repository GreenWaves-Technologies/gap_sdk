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

import ctypes
import os
import os.path
from elftools.elf.elffile import ELFFile
import time
import sys
import json_tools as js

BOOT_MODE_DEFAULT = 0
BOOT_MODE_JTAG = 1
BOOT_MODE_ROM_HYPER = 2

JTAG_SOC_INSTR_WIDTH = 0x4
JTAG_SOC_IDCODE = 0x2
JTAG_SOC_AXIREG = 0x4
JTAG_SOC_BBMUXREG = 0x5
JTAG_SOC_CLKGATEREG = 0x6
JTAG_SOC_CONFREG = 0x7
JTAG_SOC_TESTMODEREG = 0x8
JTAG_SOC_BISTREG = 0x9
JTAG_SOC_BYPASS = 0xf
JTAG_SOC_IDCODE_WIDTH = 32
JTAG_SOC_AXIREG_WIDTH = 96
JTAG_SOC_BBMUXREG_WIDTH = 21
JTAG_SOC_CLKGATEREG_WIDTH = 11
JTAG_SOC_CONFREG_WIDTH = 4
JTAG_SOC_TESTMODEREG_WIDTH = 4
JTAG_SOC_BISTREG_WIDTH = 20

JTAG_CLUSTER_INSTR_WIDTH = 4
JTAG_CLUSTER_IDCODE = 2
JTAG_CLUSTER_SAMPLE_PRELOAD = 1
JTAG_CLUSTER_EXTEST = 0
JTAG_CLUSTER_DEBUG = 8
JTAG_CLUSTER_MBIST = 9
JTAG_CLUSTER_BYPASS = 0xf
JTAG_CLUSTER_IDCODE_WIDTH = 32

JTAG_IDCODE_WIDTH = JTAG_CLUSTER_IDCODE_WIDTH + JTAG_SOC_IDCODE_WIDTH
JTAG_INSTR_WIDTH = JTAG_CLUSTER_INSTR_WIDTH + JTAG_SOC_INSTR_WIDTH


ADV_DBG_AXI4_MODULE = 0x20
ADV_DBG_CPU_MODULE = 0x21

ADV_DBG_AXI4_NOP = 0x0
ADV_DBG_AXI4_WRITE8 = 0x1
ADV_DBG_AXI4_WRITE16 = 0x2
ADV_DBG_AXI4_WRITE32 = 0x3
ADV_DBG_AXI4_WRITE64 = 0x4
ADV_DBG_AXI4_READ8 = 0x5
ADV_DBG_AXI4_READ16 = 0x6
ADV_DBG_AXI4_READ32 = 0x7
ADV_DBG_AXI4_READ64 = 0x8
ADV_DBG_AXI4_WREG = 0x9
ADV_DBG_AXI4_SELREG = 0xD

ADV_DBG_CPU_NOP = 0x0
ADV_DBG_CPU_WRITE = 0x3
ADV_DBG_CPU_READ = 0x7
ADV_DBG_CPU_WREG = 0x9
ADV_DBG_CPU_SELREG = 0xD

ADV_DBG_CPU_REG_STATUS = 0x0

class debug_bridge(object):
    """ Debug bridge doc. """

    def __init__(self, config_path, cable_option=None):
        self.config = js.import_config_from_file(config_path)
        path = os.path.join(os.environ.get('PULP_SDK_HOME'), 'install', 'ws', 'lib', 'libdebugbridge.so')
        self.module = ctypes.CDLL(path)
        self.module.bridge_new.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        self.module.jtag_write.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
        #self.module.jtag_shift.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_char_p)]
        with open(config_path, 'r') as config:
            self.instance = self.module.bridge_new(config.read().encode('utf-8'), cable_option.encode('utf-8'))

    def reset(self, active):
        """ Debug bridge reset.
        :param active: Reset target
        """
        self.module.reset(self.instance, active)

    def exit(self, status):
        self.module.plt_exit(self.instance, status)

    def jtag_write(self, addr, size, buffer):
        data = (ctypes.c_char * size).from_buffer(bytearray(buffer))
        self.module.jtag_write(self.instance, addr, size, data)

    def jtag_read(self, addr, size):
        data = (ctypes.c_ubyte * size)()
        self.module.jtag_read(self.instance, addr, size, data)

        result = []
        for elem in data:
            result.append(elem)

        return result

    def jtag_read32(self, addr):
        data = self.jtag_read(addr, 4)
        return int.from_bytes(bytearray(data), 'little')

    def jtag_reset(self):
        self.module.jtag_reset(self.instance)

    def jtag_idle(self):
        self.module.jtag_idle(self.instance)

    def jtag_shift_ir(self):
        self.module.jtag_shift_ir(self.instance)

    def jtag_shift_dr(self):
        self.module.jtag_shift_dr(self.instance)

    def jtag_shift_common(self, width, bufferin, noex):
        size = int((width + 7) / 8)
        if bufferin is not None:
            datain = (ctypes.c_char * size).from_buffer(bytearray(bufferin))
        else:
            datain = (ctypes.c_char * size)()
        dataout = (ctypes.c_ubyte * size)()
        self.module.jtag_shift(self.instance, width, datain, dataout, noex)

        result = []
        for elem in dataout:
          result.append(elem)

        return bytearray(result)

    def jtag_shift_noex(self, width, bufferin):
        return self.jtag_shift_common(width, bufferin, 1)

    def jtag_shift(self, width, bufferin):
        return self.jtag_shift_common(width, bufferin, 0)

    def jtag_set_reg(self, reg, value):
        # Set TAP in confreg mode
        self.jtag_shift_ir()
        self.jtag_shift(JTAG_SOC_INSTR_WIDTH, [reg])
        self.jtag_idle()

        # And push the new value
        self.jtag_shift_dr()
        self.jtag_shift(9, [value, 0])
        self.jtag_idle()

    def jtag_get_reg(self, reg, value):
        # Set TAP in confreg mode
        #self.jtag_shift_ir()
        #self.jtag_shift(JTAG_SOC_INSTR_WIDTH, [reg])
        #self.jtag_idle()
        #print (binascii.hexlify(bytearray(data)))

        # And push the new value
        self.jtag_shift_dr()
        result = self.jtag_shift(9, [value, 0])
        self.jtag_idle()

        return result

    def jtag_set_dbg_mode_soc(self):

        self.jtag_shift_ir()
        self.jtag_shift_noex(JTAG_CLUSTER_INSTR_WIDTH, [JTAG_SOC_BYPASS])
        self.jtag_shift(JTAG_SOC_INSTR_WIDTH, [JTAG_SOC_AXIREG])
        self.jtag_idle()

    def wait_exit(self):
        self.jtag_set_dbg_mode_soc()

        while True:
            value = self.jtag_read32(0x1a1040a0)

            if (value >> 31) & 1:
                return value & 0x7fffffff

            time.sleep(0.5)

    def wait_exit_and_stop(self):
        status = self.wait_exit()
        self.exit(status)

    def load(self, binary):

        print ('Loading ELF file (path: %s)' % binary)

        self.jtag_set_dbg_mode_soc()

        with open(binary, 'rb') as file:
            elffile = ELFFile(file)

            for segment in elffile.iter_segments():
                if segment['p_type'] == 'PT_LOAD':

                    data = segment.data()
                    addr = segment['p_paddr']
                    size = len(data)

                    print ('Loading section (base: 0x%x, size: 0x%x)' % (addr, size))

                    self.jtag_write(addr, size, data)

    def wait_and_exit(self):

        status = self.wait_exit_and_stop()
        sys.exit(status)


    def rom_boot(self):

        # This is the default mode so just let the core boot and behave in the 
        # default mode
        self.jtag_reset()
        self.reset(False)


    def rom_hyper_boot(self, binary):

        self.jtag_reset()

        # Reset the chip and tell him we want to load via jtag
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        self.reset(True)
        self.jtag_set_reg(JTAG_SOC_CONFREG, BOOT_MODE_ROM_HYPER << 1)
        self.reset(False)

        # We don't have anything else to do as it reads the boot mode
        # and does everything with the flash


    def jtag_boot(self, binary):

        self.jtag_reset()

        # Reset the chip and tell him we want to load via jtag
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        self.reset(True)
        self.jtag_set_reg(JTAG_SOC_CONFREG, BOOT_MODE_JTAG << 1)
        self.reset(False)

        # Load the binary through jtag
        self.load(binary)

        # And notify the boot code that the binary is ready
        self.jtag_set_reg(JTAG_SOC_CONFREG, (BOOT_MODE_JTAG << 1) | 1)

    def exec_config(self):

        mode = self.config.get('loader/boot/mode').get()

        if mode == 'rom':
            self.rom_boot()
        elif mode == 'rom_hyper':
            self.rom_hyper_boot()
        elif mode == 'jtag':
            self.jtag_boot(self.config.get('loader/binaries').get_elem(0).get())
        else:
            raise Exception("Unsupported mode: " + mode)

        self.wait_and_exit()
