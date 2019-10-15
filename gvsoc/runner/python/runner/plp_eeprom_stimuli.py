#!/usr/bin/env python3

# 
# Copyright (C) 2015 ETH Zurich and University of Bologna and
# GreenWaves Technologies
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#
# Authors: Germain Haugou (germain.haugou@gmail.com)
#

import os
import struct
from elftools.elf.elffile import ELFFile
import subprocess




class BinarySegment(object):

    def __init__(self, base, data):
        self.base = base
        self.data = data
        self.size = len(data)

class Boot_callback(object):

    def __init__(self, elf=None):
        self.segments = []

        if elf != None:

            callback, path = elf.split(':')
            self.callback = int(callback)

            with open(path, 'rb') as file:
                elffile = ELFFile(file)
    
                self.entry = elffile['e_entry']

                for segment in elffile.iter_segments():
                    if segment['p_type'] == 'PT_LOAD':
                        self.segments.append(BinarySegment(segment['p_paddr'], segment.data()))


class Image(object):

    def __init__(self, raw=None, verbose=True, encrypt=False, aesKey=None, aesIv=None):

        self.raw = raw
        self.buff = []
        self.eeprom_offset = 0
        self.verbose = verbose
        self.encrypt = encrypt
        self.aesKey = aesKey
        self.aesIv = aesIv
        self.boot_callbacks = []



    def add_boot_callback(self, callback):
        self.boot_callbacks.append(Boot_callback(elf=callback))
    
    def __roundToNextBlock(self):
        nextOffset = (int)((self.eeprom_offset + self.blockSize - 1) / self.blockSize) * self.blockSize
        padding = nextOffset - self.eeprom_offset
        self.eeprom_offset = nextOffset
        for i in range(0, padding):
            self.buff.append(0)

    def __padBlock(self, size):
        padsize = (int)((size + self.blockSize - 1) / self.blockSize) * self.blockSize - size
        self.eeprom_offset += padsize
        for i in range(0, padsize):
            self.buff.append(0)

    def __pad(self, padsize, buff=None):
        if buff is None:
            self.eeprom_offset += padsize
            for i in range(0, padsize):
                self.buff.append(0)
        else:
            for i in range(0, padsize):
                buff = self.__appendByte(0, buff=buff)
            return buff

    def __appendInt(self, value, newBlock=False, buff=None):
        if buff is None:
            #if newBlock: self.__roundToNextBlock()
            self.buff += struct.pack("I", value)
            self.eeprom_offset += 4
        else:
            buff += struct.pack("I", value)
            return buff

    def __appendByte(self, value, newBlock=False, buff=None):
        if buff is None:
            #if newBlock: self.__roundToNextBlock()
            self.buff += struct.pack("B", value)
            self.eeprom_offset += 1
        else:
            buff += struct.pack("B", value)
            return buff


    def get_crc(self, buff):
        crc = 0xffffffff
        for data in buff:
            crc = crc ^ data
            for i in range(7, -1, -1):
                if crc & 1 == 1:
                    mask = 0xffffffff
                else:
                    mask = 0
                crc2 = crc >> 1
                crc = (crc >> 1) ^ (0xEDB88320 & mask)

        return (crc ^ 0xffffffff)


    def __appendBuffer(self, buffer, newBlock=False, pad=False, encrypt=False, padToOffset=None):
        #if newBlock: self.__roundToNextBlock()
        if self.encrypt:
            cmd = 'aes_encode %s %s' % (self.aesKey, self.aesIv)

            crc = self.get_crc(buffer)
            buffer += struct.pack("I", crc)

            p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
            out, err = p.communicate(buffer)
            if p.returncode != 0:
                raise Exception('Error when executing aes_encore to encrypt binary, probably this tool is not available')
            buffer = out

        if padToOffset != None:
            self.__pad(padToOffset - self.eeprom_offset)

        self.buff += buffer
        self.eeprom_offset += len(buffer)
        if pad:
            self.__padBlock(len(buffer))



    def __dumpToBuff(self):

        callbacks = [None] * 32

        for callback in self.boot_callbacks:
            callbacks[callback.callback] = callback

        header_buff = bytes([])

        # Reserve one more entry to store CRC
        callbacks_offset = 4 * 4 * 33

        for callback in callbacks:
            i2c_addr = 0
            l2_addr = 0
            i2c_size = 0
            entry = 0

            if callback is not None:
                callbacks_offset = (callbacks_offset + 127) & ~0x7f
                callback.i2c_addr = callbacks_offset
                i2c_addr = callback.i2c_addr
                l2_addr = callback.segments[0].base
                i2c_size = callback.segments[0].size + 4
                entry = callback.entry

                callbacks_offset += callback.segments[0].size

            header_buff = self.__appendInt(i2c_addr, buff=header_buff)
            header_buff = self.__appendInt(l2_addr, buff=header_buff)
            header_buff = self.__appendInt(i2c_size, buff=header_buff)
            header_buff = self.__appendInt(entry, buff=header_buff)

        self.__appendBuffer(header_buff, encrypt=self.encrypt)

        for callback in callbacks:
            if callback is not None:
                self.__pad(callback.i2c_addr - self.eeprom_offset)
                self.__appendBuffer(callback.segments[0].data, encrypt=self.encrypt)



    def generate(self):

        self.__dumpToBuff()

        if self.raw != None:
            try:
                os.makedirs(os.path.dirname(self.raw))
            except:
                pass

            with open(self.raw, 'wb') as file:
                file.write(bytes(self.buff))
