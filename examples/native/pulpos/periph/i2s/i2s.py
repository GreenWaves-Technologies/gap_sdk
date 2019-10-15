#!/usr/bin/python

# Copyright (c) 2017 GreenWaves Technologies SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of GreenWaves Technologies SAS nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


import os
import sys
import time

def i2s(bridge):

    class state:
        addrWavOutHead = 0
        initialized = False
        started = False
        recReady = False

    def init_state():
        state.started = False
        state.recReady = False

    def do_i2s():
        if not state.initialized:
            print ("i2s.py execution on-going")
            addrHead = os.popen("riscv32-unknown-elf-nm test |grep -w 'I2sOutHeader$'").read()
            state.addrWavOutHead = (addrHead.split())[0]
            state.initialized = True
            return 3000000

        if not state.started:
            input("Press return to start sound acquisition:")
            micReady = int(("0x"+state.addrWavOutHead), 16) + 12
            bridge.write_32(micReady, 1)
            state.started = True
            return 0

        if not state.recReady:
            recReady = int(("0x"+state.addrWavOutHead), 16) + 8
            WavOutFlag = bridge.read_32(recReady)
            if (int(WavOutFlag) == 0):
                print ("py: waiting recReady ")
                return 1000000
            else:
                l2Addr = int(("0x"+state.addrWavOutHead), 16)
                addrWav = bridge.read_32(l2Addr)
                size = int(("0x"+state.addrWavOutHead), 16) + 4
                WavSize = bridge.read_32(size)
                with open("wavOut.wav", 'wb') as waveOut:
                    print ("py: now download", WavSize, "bytes")
                    for byte in bridge.read(addrWav, WavSize):
                        waveOut.write(byte)

                waveOut.close()
                recReady = int(("0x"+state.addrWavOutHead), 16) + 8
                bridge.write_32(recReady, 0)
                os.system("if [ ! -f ../../../waveOut.wav ]; then ln -s BUILD/$TARGET_CHIP/GCC_RISCV/wavOut.wav ../../../wavOut.wav 1>/dev/null 2>/dev/null; fi")

                print(".wav have been saved")
                init_state()
                return 1000000

    bridge.script_cb = bridge.timer_cb_typ(do_i2s)
    bridge.module.bridge_loop_timeout(bridge.script_cb, 100000)

