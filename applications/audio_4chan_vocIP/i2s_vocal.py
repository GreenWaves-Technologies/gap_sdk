#!/usr/bin/python

# Copyright 2019 GreenWaves Technologies, SAS
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
    while (1):
        WavOutFlag = bridge.read_32(recReady)
        while ( int(WavOutFlag) == 0 ):
            WavOutFlag = bridge.read_32(recReady)
            #pcAddr = int(("0x1b302000"), 16)
            #pc = bridge.read_32(pcAddr)
            #ppcAddr = int(("0x1b302004"), 16)
            #ppc = bridge.read_32(ppcAddr)
            #print ("py: waiting recReady pc=", hex(pc), "ppc=", hex(ppc))
            
        l2AddrVoc = int(("0x"+addrWavOutHead), 16) + 28
        addrWav = bridge.read_32(l2AddrVoc)
        sizeVoc = int(("0x"+addrWavOutHead), 16) + 32
        WavSize = bridge.read_32(sizeVoc)
        startidxvoc  = int(("0x"+addrWavOutHead), 16) + 36
        startidx = bridge.read_32(startidxvoc)
        with open("wavOutVoc.wav", 'wb') as waveOut:
            print ("py: now download wavOutVoc.wav", WavSize, "bytes", "from", hex(addrWav)," idx", int(startidx))
            for byte in bridge.read(addrWav, 44):
                waveOut.write(byte)
            for byte in bridge.read(addrWav+startidx+44, WavSize-44):
                waveOut.write(byte)
                #        for byte in bridge.read(addrWav+44, startidx):
                #            waveOut.write(byte)
            waveOut.closed
        print("ok wav1")

        if 0:
        
            bridge.write_32(recReady, 0)
            WavOutFlag = bridge.read_32(recReady)
            while ( int(WavOutFlag) == 0 ):
                #time.sleep(1)
                WavOutFlag = bridge.read_32(recReady)
            
            # dump direct microphone outputs
            l2AddrMicBuf = int(("0x"+addrWavOutHead), 16) + 40
            addrWav1 = bridge.read_32(l2AddrMicBuf)
            sizeVoc2 = int(("0x"+addrWavOutHead), 16) + 48
            WavSize2 = bridge.read_32(sizeVoc2)
            with open("micOut1.wav", 'wb') as waveOut1:
                print ("py: now download micOut1.wav", WavSize2, "bytes", "from", hex(addrWav1))
                for byte in bridge.read(addrWav1, WavSize2):
                    waveOut1.write(byte)
                waveOut1.closed
                    
            l2AddrMicBuf = int(("0x"+addrWavOutHead), 16) + 44
            addrWav2 = bridge.read_32(l2AddrMicBuf)
            with open("micOut2.wav", 'wb') as waveOut2:
                print ("py: now download micOut2.wav", WavSize2, "bytes", "from", hex(addrWav2))
                for byte in bridge.read(addrWav2, WavSize2):
                    waveOut2.write(byte)
                waveOut2.closed                        

                        
        os.system("if [ ! -f ../../../waveOutVoc.wav ]; then ln -s BUILD/GAP8/GCC_RISCV/wavOutVoc.wav ../../../wavOutVoc.wav 1>/dev/null 2>/dev/null; fi")
        os.system("if [ ! -f ../../../micOut1.wav ]; then ln -s BUILD/GAP8/GCC_RISCV/micOut1.wav ../../../micOut1.wav 1>/dev/null 2>/dev/null; fi")
        os.system("if [ ! -f ../../../micOut2.wav ]; then ln -s BUILD/GAP8/GCC_RISCV/micOut2.wav ../../../micOut2.wav 1>/dev/null 2>/dev/null; fi")
        
        os.system("vlc wavOutVoc.wav 1>/dev/null 2>/dev/null &")

        time.sleep(2)
        bridge.write_32(recReady, 0)


        
    exit(0)
