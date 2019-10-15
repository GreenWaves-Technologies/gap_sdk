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

from bridge.default_debug_bridge import *
import time
import math
import argparse

JTAG_SOC_AXIREG = 4

JTAG_SOC_CONFREG = 7
JTAG_SOC_CONFREG_WIDTH = 4

BOOT_MODE_JTAG = 1
BOOT_MODE_JTAG_HYPER = 11
CONFREG_BOOT_WAIT = 1
CONFREG_PGM_LOADED = 1
CONFREG_INIT = 0

INFO_FUSE_OFFSET = 0
INFO2_FUSE_OFFSET = 1
AES_KEY_FUSE_OFFSET = 2
AES_IV_FUSE_OFFSET = 18
WAIT_XTAL_DELTA_FUSE_OFFSET = 26
WAIT_XTAL_MIN_FUSE_OFFSET = 28
WAIT_XTAL_MAX_FUSE_OFFSET = 29
HYPER_RDS_DELAY_FUSE_OFFSET = 30
FLL_FREQ_FUSE_OFFSET = 31
FLL_TOLERANCE_FUSE_OFFSET = 32
FLL_ASSERT_CYCLES_FUSE_OFFSET = 33
USER_FUSE_OFFSET = 48
USER_FUSE_SIZE = 80

FUSES_ORD = [
    "INFO",
    "PLT",
    "BOOT",
    "ENCRYPTED",
    "WAIT_XTAL",
    "INFO2",
    "FLL_FREQ_SET",
    "FLL_CONF",
    "FLL_BYPASS_LOCK",
    "SPIM_CLKDIV",
    "AES_KEY",
    "AES_IV",
    "WAIT_XTAL_DELTA",
    "WAIT_XTAL_MAX",
    "WAIT_XTAL_MIN",
    "HYPER_RDS_DELAY",
    "FLL_FREQ",
    "FLL_TOLERANCE",
    "FLL_ASSERT_CYCLES",
]

FUSES = {
    "INFO": {
        "fuse_offset": INFO_FUSE_OFFSET * 8,
        "bit_len": 8,
        "format": "binary",
        "writable": False
    },
    "PLT": {
        "fuse_offset": (INFO_FUSE_OFFSET * 8) + 5, # Offset is from bit 7 in all cases
        "bit_len": 3,
        "format": "binary",
        "writable": False
    },
    "BOOT": {
        "fuse_offset": (INFO_FUSE_OFFSET * 8) + 2,
        "bit_len": 3,
        "format": "binary",
        "writable": True,
        "danger": True
    },
    "ENCRYPTED": {
        "fuse_offset": (INFO_FUSE_OFFSET * 8) + 1,
        "bit_len": 1,
        "format": "binary",
        "writable": True,
        "danger": True
    },
    "WAIT_XTAL": {
        "fuse_offset": (INFO_FUSE_OFFSET * 8) + 0,
        "bit_len": 1,
        "format": "binary",
        "writable": True,
        "danger": True
    },
    "INFO2": {
        "fuse_offset": INFO2_FUSE_OFFSET * 8,
        "bit_len": 8,
        "format": "binary",
        "writable": False
    },
    "FLL_FREQ_SET": {
        "fuse_offset": (INFO2_FUSE_OFFSET * 8) + 7,
        "bit_len": 1,
        "format": "binary",
        "writable": True,
        "danger": True
    },
    "FLL_CONF": {
        "fuse_offset": (INFO2_FUSE_OFFSET * 8) + 6,
        "bit_len": 1,
        "format": "binary",
        "writable": True,
        "danger": True
    },
    "FLL_BYPASS_LOCK": {
        "fuse_offset": (INFO2_FUSE_OFFSET * 8) + 5,
        "bit_len": 1,
        "format": "binary",
        "writable": True,
        "danger": True
    },
    "SPIM_CLKDIV": {
        "fuse_offset": (INFO2_FUSE_OFFSET * 8) + 4,
        "bit_len": 1,
        "format": "binary",
        "writable": True,
        "danger": True
    },
    "AES_KEY": {
        "fuse_offset": AES_KEY_FUSE_OFFSET * 8,
        "bit_len": 16 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "AES_IV": {
        "fuse_offset": AES_IV_FUSE_OFFSET * 8,
        "bit_len": 8 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "WAIT_XTAL_DELTA": {
        "fuse_offset": WAIT_XTAL_DELTA_FUSE_OFFSET * 8,
        "bit_len": 2 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "WAIT_XTAL_MIN": {
        "fuse_offset": WAIT_XTAL_MIN_FUSE_OFFSET * 8,
        "bit_len": 1 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "WAIT_XTAL_MAX": {
        "fuse_offset": WAIT_XTAL_MAX_FUSE_OFFSET * 8,
        "bit_len": 1 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "HYPER_RDS_DELAY": {
        "fuse_offset": HYPER_RDS_DELAY_FUSE_OFFSET * 8,
        "bit_len": 1 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "FLL_FREQ": {
        "fuse_offset": FLL_FREQ_FUSE_OFFSET * 8,
        "bit_len": 1 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "FLL_TOLERANCE": {
        "fuse_offset": FLL_TOLERANCE_FUSE_OFFSET * 8,
        "bit_len": 1 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
    "FLL_ASSERT_CYCLES": {
        "fuse_offset": FLL_ASSERT_CYCLES_FUSE_OFFSET * 8,
        "bit_len": 1 * 8,
        "format": "hex",
        "writable": True,
        "danger": True
    },
}

FEATURES={
    'target.xml':'''<?xml version="1.0"?>
<!DOCTYPE target SYSTEM "gdb-target.dtd">
<target>
  <architecture>riscv:rv32</architecture>

  <feature name="org.gnu.gdb.riscv.rv32i">
    <reg name="x0"  bitsize="32" group="general"/>
    <reg name="x1"  bitsize="32" group="general"/>
    <reg name="x2"  bitsize="32" group="general"/>
    <reg name="x3"  bitsize="32" group="general"/>
    <reg name="x4"  bitsize="32" group="general"/>
    <reg name="x5"  bitsize="32" group="general"/>
    <reg name="x6"  bitsize="32" group="general"/>
    <reg name="x7"  bitsize="32" group="general"/>
    <reg name="x8"  bitsize="32" group="general"/>
    <reg name="x9"  bitsize="32" group="general"/>
    <reg name="x10" bitsize="32" group="general"/>
    <reg name="x11" bitsize="32" group="general"/>
    <reg name="x12" bitsize="32" group="general"/>
    <reg name="x13" bitsize="32" group="general"/>
    <reg name="x14" bitsize="32" group="general"/>
    <reg name="x15" bitsize="32" group="general"/>
    <reg name="x16" bitsize="32" group="general"/>
    <reg name="x17" bitsize="32" group="general"/>
    <reg name="x18" bitsize="32" group="general"/>
    <reg name="x19" bitsize="32" group="general"/>
    <reg name="x20" bitsize="32" group="general"/>
    <reg name="x21" bitsize="32" group="general"/>
    <reg name="x22" bitsize="32" group="general"/>
    <reg name="x23" bitsize="32" group="general"/>
    <reg name="x24" bitsize="32" group="general"/>
    <reg name="x25" bitsize="32" group="general"/>
    <reg name="x26" bitsize="32" group="general"/>
    <reg name="x27" bitsize="32" group="general"/>
    <reg name="x28" bitsize="32" group="general"/>
    <reg name="x29" bitsize="32" group="general"/>
    <reg name="x30" bitsize="32" group="general"/>
    <reg name="x31" bitsize="32" group="general"/>
  </feature>
</target>
'''
}

def extract_binary(buffer, bstart, blen):
    sbuffer = []
    idx = int(math.floor(bstart / 8))
    bend = int(bstart) + int(blen)
    while idx * 8 < bend:
        rest = bend - idx * 8
        b = ord(buffer[idx])
        if rest < 8:
            sbuffer.append("{0:0{1}b}".format(b>>(8-rest), rest))
        else:
            sbuffer.append("{:08b}".format(b))
        idx = idx + 1
    return "".join(sbuffer)

def extract_hex(buffer, bytestart, bytelen):
    sbuffer = []
    for idx in range(bytestart, bytestart+bytelen-1):
        sbuffer.append("{0:02x}".format(ord(buffer[idx])))
    return "".join(sbuffer)

class gap_debug_bridge(debug_bridge):

    def __init__(self, config, binaries=[], verbose=False, fimages=[]):
        super(gap_debug_bridge, self).__init__(config=config, binaries=binaries, verbose=verbose)

        self.fimages = fimages
        self.start_cores = False
        self.boot_mode = None
        self.stopped = False

    def reset(self):
        self.get_cable().jtag_reset(True)
        self.get_cable().jtag_reset(False)
        self.get_cable().chip_reset(True)
        self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, (BOOT_MODE_JTAG << 1) | 1)
        self.get_cable().chip_reset(False)
        return True

    def set_boot_mode(self, boot_mode, reset=True):
        if self.verbose:
            self.log(0, "Notifying to boot code new boot mode (mode: %d)" % boot_mode)
        if reset:
            self.get_cable().chip_reset(True)
        self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, (boot_mode << 1) | 1)

        if reset:
            self.get_cable().chip_reset(False)
        self.boot_mode = boot_mode

    def wait_available(self):
        if self.verbose:
            self.log(0, "Waiting for target to be available")

        boot_mode = 0
        if self.boot_mode is not None:
            boot_mode = (self.boot_mode << 1) | 1

        # Loop until we see bit 0 becoming 1, this will indicate that the
        # target is ready to accept bridge requests
        while True:
            reg_value = self.get_cable().jtag_get_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, boot_mode)

            rt_req = (reg_value >> 1) & 0x7

            if rt_req == 4 or rt_req == 1:
                break

        if self.verbose:
            self.log(0, "Target is available")


    def wait_ready(self, boot_mode=None):
        if boot_mode is None:
            boot_mode = self.boot_mode

        if boot_mode is None:
            self.log(0, 'Can not wait for boot code if the boot mode is unknown')
            return False

        if self.verbose:
            self.log(0, "Waiting for notification from boot code")
        while True:
            reg_value = self.get_cable().jtag_get_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, (boot_mode << 1) | 1)

            if reg_value == CONFREG_BOOT_WAIT:
                break
        self.log(0, "Received for notification from boot code")

        return True

    def stop(self):

        # Reset the chip and tell him we want to load via jtag
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        self.set_boot_mode(BOOT_MODE_JTAG)

        # Now wait until the boot code tells us we can load the code
        if not self.wait_ready():
            return False

        # Stall the FC
        self.write(0x1B300000, 4, [0, 0, 1, 0])

        self.stopped = True

        return True

    def pause_for_conf(self):
        self.set_boot_mode(4)
        if not self.wait_ready():
            return False
        return True

    def resume_after_conf(self, boot_mode):
        self.set_boot_mode(5, reset=False)
        while True:
            reg_value = self.get_cable().jtag_get_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, (5 << 1) | 1)
            if (reg_value & 1) == 0:
                break

        self.set_boot_mode(boot_mode, reset=False)

        return True

    def clear(self):
        self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, 0)
        self.boot_mode = None


    def load_jtag(self):

        if self.verbose:
            self.log(0, 'Loading binary through jtag')

        if not self.stop():
            return False

        # Load the binary through jtag
        for binary in self.binaries:
            self.log(1, "Loading binary from " + binary)
            if not self.load_elf(binary=binary):
                return False

        # Be careful to set the new PC only after the code is loaded as the prefetch
        # buffer is immediately fetching instructions and would get wrong instructions
        self.write(0x1B302000, 4, [0x80, 0x00, 0x00, 0x1c])

        self.start_cores = True

        return True


    def start(self):

        if self.start_cores:
            self.log(0, 'Starting execution')

            # Unstall the FC so that it starts fetching instructions from the loaded binary
            self.write(0x1B300000, 4, [0, 0, 0, 0])

        return True


    def load_jtag_hyper(self, binaries):

        if self.verbose:
            self.log(0, 'Loading binary through jtag_hyper')

        # Reset the chip and tell him we want to load from hyper
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        if self.verbose:
            self.log(0, "Notifying to boot code that we are doing a JTAG boot from hyperflash")
        self.get_cable().chip_reset(True)
        self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, BOOT_MODE_JTAG_HYPER)
        self.get_cable().chip_reset(False)

        return True

    # the flash process needs to happen asynchronously since we want the loopers
    # to be running
    def flash(self, f_path):
        MAX_BUFF_SIZE = 350 * 1024
        addrHeader = self._get_binary_symbol_addr('flasherHeader')
        addrImgRdy = addrHeader
        addrFlasherRdy = addrHeader + 4
        addrFlashAddr = addrHeader + 8
        addrIterTime = addrHeader + 12
        addrBufSize = addrHeader + 16

        # open the file in read binary mode
        f_img = open(f_path, 'rb')
        f_size = os.path.getsize(f_path)

        lastSize = f_size % MAX_BUFF_SIZE
        if lastSize:
            n_iter = f_size // MAX_BUFF_SIZE + 1
        else:
            n_iter = f_size // MAX_BUFF_SIZE

        # since we need to be python 2.X compatible this holds state
        class state:
            cnt = 0
            initialized = False
            addr_buffer = None

        def flash_one():
            if self.read_32(addrFlasherRdy) == 0:
                return 0

            if not state.initialized:
                self.log(1, "Initializing bridge flasher")
                state.initialized = True
                state.addr_buffer = self.read_32((addrHeader+20))
                self.log(1, "Flash address buffer 0x{:x}".format(state.addr_buffer))
                self.write_32(addrFlashAddr, 0)
                self.write_32(addrIterTime, n_iter)

            if (lastSize and state.cnt == (n_iter-1)):
                buff_data = f_img.read(lastSize)
                self.write(state.addr_buffer, lastSize, buff_data)
                self.write_32(addrBufSize, ((lastSize + 3) & ~3))
            else:
                buff_data = f_img.read(MAX_BUFF_SIZE)
                self.write(state.addr_buffer, MAX_BUFF_SIZE, buff_data)
                self.write_32(addrBufSize, MAX_BUFF_SIZE)

            self.write_32(addrFlasherRdy, 0)
            self.write_32(addrImgRdy, 1)

            state.cnt = state.cnt + 1
            if state.cnt < n_iter:
                return 0
            else:
                f_img.close()
                self.log(1, "Bridge flasher completed")
                return -1

        # keep a reference to the callback
        self.flash_cb = self.timer_cb_typ(flash_one)
        self.module.bridge_loop_timeout(self.flash_cb, 100000)

        return True
