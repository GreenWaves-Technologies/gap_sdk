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

BOOT_MODE_JTAG = 4
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

    def __init__(self, config, binaries=[], verbose=0, fimages=[]):
        super(gap_debug_bridge, self).__init__(config=config, binaries=binaries, verbose=verbose)

        self.fimages = fimages
        self.start_cores = False
        # self.capabilities("qXfer:features:read+")

    def stop(self):
        self.is_started = False
        # Reset the chip and tell him we want to load via jtag
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        self.log(1, "Notifying to boot code that we are doing a JTAG boot")
        if (not self.get_cable().chip_reset(True)):
            self.log(0, "Failed to assert chip reset")

        self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, BOOT_MODE_JTAG)

        if (not self.get_cable().chip_reset(False)):
            self.log(0, "Failed to deassert chip reset")

        # Removed synchronization with boot code due to HW bug, it is better
        # to stop fc as soon as possible

#        # Now wait until the boot code tells us we can load the code
#        if self.verbose:
#            print ("Waiting for notification from boot code")
#        while True:
#            reg_value = self.get_cable().jtag_get_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, BOOT_MODE_JTAG)
#            if reg_value == CONFREG_BOOT_WAIT:
#                break
#        print ("Received for notification from boot code")

        # Stall the FC
        self.write(0x1B300000, 4, [0, 0, 1, 0])

        # Configure FLL with no lock to avoid the HW bug with fll
        self.write_32(0x1a100004, 0x840005f5)
        self.write_32(0x1a100008, 0x8100410b)

        return True

    def qxfer_read(self, object, annex):
        if object != "features":
            raise XferInvalidObjectException()
        if annex not in FEATURES:
            raise XferInvalidAnnexException()
        return FEATURES[annex]

    def load_jtag(self):

        self.log(1, 'Loading binary through jtag')

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
        if self.start_cores and not self.is_started:
            self.log(1, 'Start execution on GAP8')

            self.is_started = True
            # Unstall the FC so that it starts fetching instructions from the loaded binary
            return self.write(0x1B300000, 4, [0, 0, 0, 0])

        return False


    def load_jtag_hyper(self):
        self.log(1, 'Loading binary through jtag_hyper')

        # Reset the chip and tell him we want to load from hyper
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        self.log(1, "Notifying to boot code that we are doing a JTAG boot from hyperflash")
        res = self.get_cable().chip_reset(True)
        res = res and self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, BOOT_MODE_JTAG_HYPER)
        res = res and self.get_cable().chip_reset(False)
        res = res and self.write(0x1B300000, 4, [0, 0, 1, 0])

        self.start_cores = True
        return res

    def fuse_validate(self, args, do_write):
        if do_write and args.fuse_value is None:
            raise Exception("--fuse-value must be specified for a write")

        if args.fuse_offset is not None:
            if do_write and args.fuse_offset / 8 < USER_FUSE_OFFSET:
                raise Exception("--fuse-name must be used for GAP system fuses")
            if args.fuse_offset >= (USER_FUSE_OFFSET * 8) + (USER_FUSE_SIZE * 8):
                raise Exception("--fuse-offset is invalid")

        else:
            if args.fuse_name is None:
                raise Exception("--fuse-offset or --fuse-name must be specified")
            if args.fuse_name not in FUSES:
                if args.fuse_name.startswith("USER"):
                    user_loc = args.fuse_name[4:].split('_', 1)
                    if len(user_loc) == 1:
                        reg = int(user_loc[0])
                        if reg < 0 or reg >= USER_FUSE_SIZE:
                            raise Exception("user register is outside bounds")
                        fuse = { "fuse_offset": (USER_FUSE_OFFSET + reg) * 8, "bit_len": 8, "format": "hex", "writable": True }
                    elif len(user_loc) == 2:
                        reg = int(user_loc[0])
                        bit = int(user_loc[1])
                        if reg < 0 or reg >= USER_FUSE_SIZE:
                            raise Exception("user register is outside bounds")
                        if bit < 0 or bit > 7:
                            raise Exception("user register bit is outside bounds")
                        fuse = { "fuse_offset": (USER_FUSE_OFFSET + reg) * 8 + 7 - bit, "bit_len": 1, "format": "binary", "writable": True }
                    else:
                        raise Exception("invalid user register. Must be USER1 or USER1_1.")
                    if do_write:
                        args.danger = True
                elif args.fuse_name == "ALL":
                    low = 1000000
                    high = 0
                    for fn in FUSES:
                        f = FUSES[fn]
                        if f["fuse_offset"] < low:
                            low = f["fuse_offset"]
                        last = f["fuse_offset"] + f["bit_len"] - 1
                        if last > high:
                            high = last
                    fuse = { "fuse_offset": low, "bit_len": high - low + 1, "format": "all", "writable": False }
                else:
                    raise Exception("--fuse-name not found")
            else:
                fuse = FUSES[args.fuse_name]

            if do_write:
                if not args.danger:
                    raise Exception("--danger must be specified to write this fuse - !!!!!! THIS OPERATION CAN PERMANENTLY RENDER YOUR GAP8 UNBOOTABLE !!!!!!")
                if fuse["bit_len"] != args.fuse_value["bit_len"]:
                    raise Exception("--fuse-value is of incorrect length")

                if not fuse["writable"]:
                    raise Exception("you cannot burn this fuse")

            else:
                if args.fuse_format is None:
                    args.fuse_format = fuse['format']
                args.fuse_read_len = fuse["bit_len"]

            args.fuse_offset = fuse["fuse_offset"]

        if not do_write:
            if args.fuse_read_len is None:
                raise Exception("--fuse-read-len must be specified")
            if args.fuse_read_len + args.fuse_offset > (USER_FUSE_OFFSET * 8) + (USER_FUSE_SIZE * 8):
                raise Exception("--fuse-read-len is out of range")


    def print_all_fuses(self, bit_offset, buffer):
        print("All fuses")
        for fname in FUSES_ORD:
            f = FUSES[fname]
            if f['format'] == "binary":
                print("{0}: {1}".format(fname, extract_binary(buffer, bit_offset + f["fuse_offset"], f["bit_len"])))
            elif f['format'] == "hex":
                bstart = (bit_offset + f["fuse_offset"])/8
                blen = f["bit_len"]/8
                if f["bit_len"]%8 > 0:
                    blen = blen + 1
                print("{0}: {1}".format(fname, extract_binary(buffer, bstart, blen)))

    def fuse(self, bit_offset, bit_len, buffer = None, fmt = "binary"):
        self.log(1, "Fuse operation at offset {} len {}".format(bit_offset, bit_len))
        addrHeader = self._get_binary_symbol_addr('fuseHeader')
        addrFuseVerMaj = addrHeader
        addrFuseVerMin = addrHeader + 4
        addrBridgeReady = addrHeader + 8
        addrGapReady = addrHeader + 12
        addrOp = addrHeader + 16
        addrPos = addrHeader + 20
        addrLen = addrHeader + 24
        addrStatus = addrHeader + 28
        addrLogLevel = addrHeader + 32
        addrBuffer = addrHeader + 36
        bit_offset = bit_offset
        bit_len = bit_len
        fmt = fmt
        if buffer is None:
            do_write = False
        else:
            do_write = True
            write_buffer = buffer           

        class state:
            state = "init"

        def fuse_proc():
            if state.state == "init":
                if self.read_32(addrGapReady) == 0:
                    return 0
                ver_maj = self.read_32(addrFuseVerMaj)
                ver_min = self.read_32(addrFuseVerMin)
                self.write_32(addrLogLevel, self.verbose)
                self.log(0, "Fuser version {}.{} detected".format(ver_maj, ver_min))
                if ver_maj != 1:
                    self.log(1, "Fuse failed. Incorrect fuser version")
                    return -1

                state.state = "op"
            if state.state == "op":
                self.write_32(addrGapReady, 0)
                self.write_32(addrPos, bit_offset)
                self.write_32(addrLen, bit_len)
                if do_write:
                    self.write_32(addrOp, 1)
                    self.write(addrBuffer, math.ceil(bit_len/8), write_buffer)
                    self.get_cable().set_VQPS(True)
                else:
                    self.write_32(addrOp, 0)

                self.log(3, "indicate parameters set")
                self.write_32(addrBridgeReady, 1)
                state.state = "wait"
            if state.state == "wait":
                if self.read_32(addrGapReady) == 0:
                    return 0
                state.state = "result"
            if state.state == "result":
                self.log(3, "Fuser operation indicated complete")
                if do_write:
                    self.get_cable().set_VQPS(False)
                status = self.read_32(addrStatus)
                if status != 0:
                    self.log(0, "Error in fuse operation")
                elif not do_write:
                    bsize = math.ceil(bit_len / 8)
                    buffer = self.read(addrBuffer, bsize)
                    if fmt == "binary":
                        print("fuse contents: {0}".format(extract_binary(buffer, 0, bit_len)))
                    elif fmt == "all":
                        self.print_all_fuses(bit_offset, buffer)
                    else:
                        print("fuse contents: {0}".format(extract_hex(buffer, 0, math.ceil(bit_len / 8) + 1)))

                else:
                    self.log(1, "fuse written")

                state.state = "init"
                # exit fuser
                self.write_32(addrBridgeReady, 2)
                return -1

        # keep a reference to the callback
        self.fuse_cb = self.timer_cb_typ(fuse_proc)
        self.module.bridge_loop_timeout(self.fuse_cb, 100000)

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


    def load_jtag_old(self):

        self.log(1, 'Loading binary through jtag')

        # Reset the chip and tell him we want to load via jtag
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        self.log(1, "Notifying to boot code that we are doing a JTAG boot")
        self.get_cable().chip_reset(True)
        self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, BOOT_MODE_JTAG)
        self.get_cable().chip_reset(False)

        # Now wait until the boot code tells us we can load the code
        self.log(1, "Waiting for notification from boot code")
        while True:
            reg_value = self.get_cable().jtag_get_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, BOOT_MODE_JTAG)
            if reg_value == CONFREG_BOOT_WAIT:
                break
        self.log(1, "Received notification from boot code")


        # Load the binary through jtag
        self.log(1, "Loading binaries")
        for binary in self.binaries:
            if self.load_elf(binary=binary):
                return False

        return True


    def start_old(self):

        # And notify the boot code that the binary is ready
        self.log(1, "Notifying to boot code that the binary is loaded")
        self.get_cable().jtag_set_reg(JTAG_SOC_CONFREG, JTAG_SOC_CONFREG_WIDTH, CONFREG_PGM_LOADED)

        return True


