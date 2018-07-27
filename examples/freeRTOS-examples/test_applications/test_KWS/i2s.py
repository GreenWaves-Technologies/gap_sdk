#!/usr/bin/python
import os
import sys
import time

def i2s(bridge):
    print ("i2s.py execution on-going")
    addrWavOutHead = os.popen("riscv32-unknown-elf-nm test |grep -w 'i2s_out_header$'").read()
    addrWavOutHead = (addrWavOutHead.split())[0]

    while (1) :
        time.sleep(3)
        input("Press return to start sound acquisition:")
        recReady = int(("0x"+addrWavOutHead), 16) + 8
        bridge.write_32(recReady, 0)

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

        os.system("if [ ! -f ../../../waveOut.wav ]; then ln -s BUILD/GAP8/GCC_RISCV/wavOut.wav ../../../wavOut.wav 1>/dev/null 2>/dev/null; fi")
        time.sleep(1)
