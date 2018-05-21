#!/usr/bin/python
#
# Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#

import os
import sys
import time
from PIL import Image

def camera(bridge):
    addrWavOutHead = os.popen("riscv32-unknown-elf-nm test |grep -w 'ImageOutHeader$'").read()
    addrWavOutHead = (addrWavOutHead.split())[0]
    time.sleep(3)
    input("Py: Press Enter to acquire an image:")
    inReady = int(("0x"+addrWavOutHead), 16) + 12
    bridge.write_32(inReady, 1)
    outReady = int(("0x"+addrWavOutHead), 16) + 8
    WavOutFlag = bridge.read_32(outReady)
    while ( int(WavOutFlag) == 0 ):
        time.sleep(1)
        outReady = int(("0x"+addrWavOutHead), 16) + 8
        WavOutFlag = bridge.read_32(outReady)
        print ("Py: Waiting image to be written")
    l2Addr = int(("0x"+addrWavOutHead), 16)
    addr = bridge.read_32(l2Addr)
    size_addr = int(("0x"+addrWavOutHead), 16) + 4
    size = bridge.read_32(size_addr)
    with open("../../../imgOut.pgm", 'wb') as waveOut:
        print ("Py: Saving image to file ... ( ", size, " bytes)")
        for byte in bridge.read(addr, size):
            waveOut.write(byte)
    waveOut.closed
    outReady = int(("0x"+addrWavOutHead), 16) + 8
    bridge.write_32(outReady, 0)
