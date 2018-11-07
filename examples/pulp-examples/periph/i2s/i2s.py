#!/usr/bin/python
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
                os.system("if [ ! -f ../../../waveOut.wav ]; then ln -s BUILD/GAP8/GCC_RISCV/wavOut.wav ../../../wavOut.wav 1>/dev/null 2>/dev/null; fi")

                print(".wav have been saved")
                init_state()
                return 1000000

    bridge.script_cb = bridge.timer_cb_typ(do_i2s)
    bridge.module.bridge_loop_timeout(bridge.script_cb, 100000)

