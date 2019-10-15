#!/usr/bin/env python3

# 
# Copyright (C) 2015 ETH Zurich and University of Bologna
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



def dumpByteToSlm(file, addr, value):
    file.write("@%08X %02X\n" % (addr, value))

def dumpShortToSlm(file, addr, value):
    file.write("@%08X %04X\n" % (addr, value))

def dump_word( filetoprint, addr, data_s):
    for i in xrange(0,4,1):
        filetoprint.write("@%08X %s\n" % ( addr+i,  data_s[i*2:(i+1)*2] ))
    return 4

class Comp(object):

    def __init__(self, dirpath, name):
        self.path = os.path.join(dirpath, name)
        self.name = name
        self.size = os.path.getsize(self.path)

    def dump(self):
        print (self.name)
        print ('Path: ' + self.path)
        print ('Flash addr: ' + str(self.flashAddr))
        print ('Size: ' + str(self.size))
        print ('')

class BinarySegment(object):

    def __init__(self, base, data):
        self.base = base
        self.data = data
        self.size = len(data)

class Binary(object):

    def __init__(self, elf=None):
        self.segments = []

        if elf != None:

            with open(elf, 'rb') as file:
                elffile = ELFFile(file)
    
                self.entry = elffile['e_entry']

                for segment in elffile.iter_segments():
                    if segment['p_type'] == 'PT_LOAD':
                        self.segments.append(BinarySegment(segment['p_paddr'], segment.data()))


class FlashImage(object):

    def __init__(self, raw=None, stimuli=None, verbose=True, archi=None, encrypt=False, aesKey=None, aesIv=None, flashType='spi', qpi=True):

        self.bootBinary = None
        self.raw = raw
        self.stimuli = stimuli
        self.compList = []
        self.buff = []
        self.flashOffset = 0
        if flashType == 'hyper': self.blockSize = 1024
        else: self.blockSize = 4096
        self.bootaddr = 0x1c000000
        self.verbose = verbose
        self.archi = archi
        self.encrypt = encrypt
        self.aesKey = aesKey
        self.aesIv = aesIv
        self.flashType = flashType
        self.qpi = qpi



    def appendBootBinary(self, elf=None):
        self.bootBinary = Binary(elf=elf)
    
    def appendComponent(self, dirname, name):

        self.compList.append(Comp(dirname, name))

    def __roundToNextBlock(self):
        nextOffset = (int)((self.flashOffset + self.blockSize - 1) / self.blockSize) * self.blockSize
        padding = nextOffset - self.flashOffset
        self.flashOffset = nextOffset
        for i in range(0, padding):
            self.buff.append(0)

    def __padBlock(self, size):
        padsize = (int)((size + self.blockSize - 1) / self.blockSize) * self.blockSize - size
        self.flashOffset += padsize
        for i in range(0, padsize):
            self.buff.append(0)

    def __pad(self, padsize):
        self.flashOffset += padsize
        for i in range(0, padsize):
            self.buff.append(0)

    def __appendInt(self, value, newBlock=False):
        #if newBlock: self.__roundToNextBlock()
        self.buff += struct.pack("I", value)
        self.flashOffset += 4

    def __appendByte(self, value, newBlock=False):
        #if newBlock: self.__roundToNextBlock()
        self.buff += struct.pack("B", value)
        self.flashOffset += 1

    def __appendBuffer(self, buffer, newBlock=False, pad=False, encrypt=False, padToOffset=None):
        #if newBlock: self.__roundToNextBlock()
        if self.encrypt:
            cmd = 'aes_encode %s %s' % (self.aesKey, self.aesIv)

            p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
            out, err = p.communicate(buffer)
            if p.returncode != 0:
                raise Exception('Error when executing aes_encore to encrypt binary, probably this tool is not available')
            buffer = out

        if padToOffset != None: self.__pad(padToOffset - self.flashOffset)
        self.buff += buffer
        self.flashOffset += len(buffer)
        if pad: self.__padBlock(len(buffer))



    def __dumpToBuff(self):
        self.__dumpBootBinaryToBuff()
        self.__dumpCompsToBuff()

    def __dumpFlashHeader_v1(self):

        if self.verbose:
            print ("Generating boot binary")

        index = 0
        print ("Merging following segments:")
        for segment in self.bootBinary.segments:
            if self.verbose: print ("  Area %d: base: 0x%x, size: 0x%x" % (index, segment.base, segment.size))
            index = index + 1

        l1Area =  BinarySegment(0x10000000, [])
        l2Area =  BinarySegment(0x1c000000, [])

        self.bootBinary.mergedSegments = [l2Area, l1Area]

        for segment in self.bootBinary.segments:
            if segment.base < 0x1c000000: area = l1Area
            else: area = l2Area
            if segment.base > area.base + area.size:
                diff = segment.base - (area.base + area.size)
                area.size += diff
                for i in range(0, diff): 
                    area.data.append(0)

            area.data += segment.data
            area.size += segment.size

        # First compute areas flash information
        flashOffset = 0
        flashOffset += 4*4*2

        print ("Merged into:")
        index = 0
        for segment in self.bootBinary.mergedSegments:
            segment.nbBlocks = int((segment.size + self.blockSize - 1) / self.blockSize)
            segment.offset = flashOffset
            flashOffset += segment.nbBlocks * self.blockSize
            if self.verbose: print ("  Area %d: offset: 0x%x, base: 0x%x, size: 0x%x, nbBlocks: %d" % (index, segment.offset, segment.base, segment.size, segment.nbBlocks))
            index += 1

        # Then write the header containing memory areas declaration
        self.__appendInt(l2Area.offset)
        self.__appendInt(l2Area.base)
        self.__appendInt(l2Area.size)
        self.__appendInt(l2Area.nbBlocks)

        self.__appendInt(l1Area.offset)
        self.__appendInt(l1Area.base)
        self.__appendInt(l1Area.size)
        self.__appendInt(l1Area.nbBlocks)

        # Finally write the data
        for area in self.bootBinary.mergedSegments:
            self.__appendBuffer(area.data, pad=True, encrypt=self.encrypt)

    def __dumpFlashHeader_v2(self):

        if self.verbose:
            print ("Generating boot binary")
            print ("  Nb areas: %d" % len(self.bootBinary.segments))

        # First compute areas flash information
        flashOffset = 0
        flashOffset += 4 + 4 + 4 + 4 + len(self.bootBinary.segments) * 4 * 4

        flashOffset = (flashOffset + self.blockSize - 1) & ~(self.blockSize - 1)

        index = 0
        for segment in self.bootBinary.segments:
            segment.nbBlocks = int((segment.size + self.blockSize - 1) / self.blockSize)
            segment.offset = flashOffset
            flashOffset += segment.nbBlocks * self.blockSize
            if self.verbose: print ("  Area %d: offset: 0x%x, base: 0x%x, size: 0x%x, nbBlocks: %d" % (index, segment.offset, segment.base, segment.size, segment.nbBlocks))
            index += 1


        # Then write the header containing memory areas declaration
        self.fsOffset = flashOffset
        self.__appendInt(flashOffset)
        self.__appendInt(len(self.bootBinary.segments))
        #self.__appendInt(self.bootBinary.entry)
        self.__appendInt(self.bootBinary.entry)
        self.__appendInt(self.bootaddr)

        for area in self.bootBinary.segments:
            self.__appendInt(area.offset)
            self.__appendInt(area.base)
            self.__appendInt(area.size)
            self.__appendInt(area.nbBlocks)

        # Finally write the data
        for area in self.bootBinary.segments:
            self.__appendBuffer(area.data, padToOffset=area.offset, encrypt=self.encrypt)

        self.__pad(self.fsOffset - self.flashOffset)

    def __dumpBootBinaryToBuff(self):
        if self.bootBinary != None:

            if self.archi == 'vivosoc2' or self.archi == 'fulmine':
                self.__dumpFlashHeader_v1()
            else:
                self.__dumpFlashHeader_v2()

        else:
            # In case no boot binary is there, we must have at least the first word telling where starts the next descriptor
            self.__appendInt(4)


    def __dumpCompsToBuff(self):

        #
        #  Flash address computation
        #
        
        if self.verbose: print ('Generating files (header offset: 0x%x)' % self.flashOffset)

        flashAddr = self.flashOffset
        headerSize = 0
        
        # Compute the header size
        headerSize += 8    # Header size and number of components
        
        for comp in self.compList:
            headerSize += 12                # Flash address, size and path length
            headerSize += len(comp.name)+1  # Path
        
        flashAddr += headerSize
        
        # Now set the flash address for each component
        for comp in self.compList:
            comp.flashAddr = (flashAddr + 3) & ~3
            if self. verbose:
                print ('  Adding component (name: %s, flashOffset: 0x%x)' % (comp.name, comp.flashAddr))
            flashAddr = comp.flashAddr + comp.size


        # Now create the raw image as a byte array
        
        # First header size
        self.__appendInt(headerSize)
        
        # Number of components
        self.__appendInt(len(self.compList))
        
        # Then for each component
        for comp in self.compList:
            # The flash address
            self.__appendInt(comp.flashAddr)
        
            # Binary size
            self.__appendInt(comp.size)
        
            # The path length
            self.__appendInt(len(comp.name)+1)
        
            # And the path
            self.__appendBuffer(comp.name.encode('utf-8'))
            self.__appendByte(0)
        
        # Then dump all components
        for comp in self.compList:
            with open(comp.path, 'rb') as file:
                self.__appendBuffer(file.read(), padToOffset=comp.flashAddr)



    def generate(self):

        self.__dumpToBuff()

        if self.raw != None:
            with open(self.raw, 'wb') as file:
                file.write(self.buff)

        if self.stimuli != None:
            with open(self.stimuli, 'w') as file:
                if self.flashType == 'hyper':
                    if len(self.buff) & 1 != 0:
                        self.__appendByte(0)
                    for i in range(0, len(self.buff)>>1):
                        value = (self.buff[i*2+1] << 8) + self.buff[i*2]
                        dumpShortToSlm(file, i, value)
                elif self.archi == 'vivosoc2' or self.archi == 'fulmine':
                    if len(self.buff) % 4 != 0:
                        for i in range(0, 4 - (len(self.buff)%4)):
                            self.buff.append(0)
                    for i in range(0, len(self.buff), 4):
                        dumpByteToSlm(file, i, self.buff[i+3])
                        dumpByteToSlm(file, i+1, self.buff[i+2])
                        dumpByteToSlm(file, i+2, self.buff[i+1])
                        dumpByteToSlm(file, i+3, self.buff[i+0])
                else:
                    for i in range(0, len(self.buff)):
                        dumpByteToSlm(file, i, self.buff[i])


def genFlashImage(slmStim=None, bootBinary=None, comps=[], verbose=False, archi=None, encrypt=False, aesKey=None, aesIv=None, flashType='spi', qpi=True):
    if bootBinary != None or len(comps) != 0:
        if slmStim != None:
            compsList = ''
            romBoot = ''

            if bootBinary != None:
                romBoot = ' --flash-boot-binary=%s' % bootBinary
    
            for comp in comps:
                compsList += ' --comp=%s' % comp
    
            cmd = "plp_mkflash %s %s --stimuli=%s --flash-type=%s" % (romBoot, compsList, slmStim, flashType)

            if qpi: cmd+= ' --qpi'

            if encrypt: cmd += ' --encrypt --aes-key=%s --aes-iv=%s' % (aesKey, aesIv)

            if verbose: cmd += ' --verbose'
            if archi != None: cmd += ' --archi=%s' % archi
            if verbose:
                print ('Building flash stimuli with command:')
                print (cmd)
            if os.system(cmd) != 0:
                raise Exception('Error while generating flash image')


    return 0