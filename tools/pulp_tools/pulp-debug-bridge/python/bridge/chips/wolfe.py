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

class wolfe_debug_bridge(debug_bridge):

    def __init__(self, config, binaries=[], verbose=0):

        super(wolfe_debug_bridge, self).__init__(config=config, binaries=binaries, verbose=verbose)

        self.start_cores = False


    def load_jtag(self):

        self.log(1, 'Loading binary through jtag')

        if not self.stop():
            return False

        # Load the binary through jtag
        self.log(1, "Loading binaries")
        for binary in self.binaries:
            if not self.load_elf(binary=binary):
                return False

        # Be careful to set the new PC only after the code is loaded as the prefetch
        # buffer is immediately fetching instructions and would get wrong instructions
        self.write(0x1A112000, 4, [0x80, 0x80, 0x00, 0x1c])

        self.start_cores = True

        return True


    def start(self):

        if self.start_cores:
            # Unstall the FC so that it starts fetching instructions from the loaded binary
            return self.write(0x1A110000, 4, [0, 0, 0, 0])

        return False

    def stop(self):

        # Reset the chip and tell him we want to load via jtag
        # We keep the reset active until the end so that it sees
        # the boot mode as soon as it boots from rom
        if self.verbose:
            print ("Stalling the FC")

        self.get_cable().chip_reset(True)
        self.get_cable().chip_reset(False)

        # Stall the FC as when the reset is released it just tries to load from flash
        while True:
            # on Wolfe, the core will start only after a while when the reset
            # is released, so the first accesses we do may not have any effect.
            # As a aworkaround we have to try stopping it several time.
            # Another issue on RTL simulation is that the core is not
            # functional anymore if we let him execute a branch with X
            # which happens if we let it run too long before we stop it.
            # Due to that we cannot read the stall status before we stop it.
            for i in range(0, 10):
                self.write(0x1A110000, 4, [0, 0, 1, 0])

            value = self.read_32(0x1A110000)
            if (value >> 16) & 1 == 1:
                break

        return True
