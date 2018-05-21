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
    addrWavOutHead = os.popen("riscv32-unknown-elf-nm test |grep -w 'I2sOutHeader$'").read()
    addrWavOutHead = (addrWavOutHead.split())[0]

    cStarted = int(("0x"+addrWavOutHead), 16) + 16
    st = bridge.read_32(cStarted)
    while ( int(st) == 0 ):
        print ("py: wait c code to start")
        time.sleep(1)
        cStarted = int(("0x"+addrWavOutHead), 16) + 16
        st = bridge.read_32(cStarted)

    input("py: Press return to start sound acquisition:")

    micReady = int(("0x"+addrWavOutHead), 16) + 12
    bridge.write_32(micReady, 1)

    recReady = int(("0x"+addrWavOutHead), 16) + 8
    WavOutFlag = bridge.read_32(recReady)
    while ( int(WavOutFlag) == 0 ):
        time.sleep(1)
        recReady = int(("0x"+addrWavOutHead), 16) + 8
        WavOutFlag = bridge.read_32(recReady)
        print ("py: waiting recReady")

    l2Addr = int(("0x"+addrWavOutHead), 16)
    addrWav = bridge.read_32(l2Addr)
    size = int(("0x"+addrWavOutHead), 16) + 4
    WavSize = bridge.read_32(size)
    with open("wavOut.wav", 'wb') as waveOut:
        print ("py: now download", WavSize, "bytes")
        for byte in bridge.read(addrWav, WavSize):
            waveOut.write(byte)

    waveOut.closed

    recReady = int(("0x"+addrWavOutHead), 16) + 8
    bridge.write_32(recReady, 0)

    os.system("if [ ! -f ../../../waveOut.wav ]; then ln -s BUILD/GAP8/GCC_RISCV/wavOut.wav ../../../wavOut.wav 1>/dev/null 2>/dev/null; fi")
    os.system("vlc wavOut.wav 1>/dev/null 2>/dev/null &")
