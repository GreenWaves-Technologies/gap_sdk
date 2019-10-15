#!/usr/bin/python

# ////////////////////////////////////////////////////////////////////////////////
# // Company:        Multitherman Laboratory @ DEIS - University of Bologna     //
# //                    Viale Risorgimento 2 40136                              //
# //                    Bologna - fax 0512093785 -                              //
# //                                                                            //
# // Engineer:       Davide Rossi - davide.rossi@unibo.it                       //
# //                                                                            //
# // Additional contributions by:                                               //
# //                 Andreas Traber - atraber@student.ethz.ch                   //
# //                                                                            //
# // Create Date:    05/04/2013                                                 //
# // Design Name:    ULPSoC                                                     //
# // Project Name:   ULPSoC                                                     //
# // Language:       tcl, now python                                            //
# //                                                                            //
# // Description:    s19 to slm conversion tool for stxp70 cluster compilation  //
# //                                                                            //
# // Revision:                                                                  //
# // Revision v0.1 - File Created                                               //
# // Revision v0.2 - Modification: Compiler does now generate little endian     //
# //                 directly. revert bytes!                                    //
# // Revision v0.3 - Moved from 128 bit s19 to 8 bit s19 file. This solves our  //
# //                 problems with misaligned addresses in s19 files.           //
# // Revision v0.4 - Added TCDM memory initialization                           //
# // Revision v0.5 - Rewrote the whole thing in python as tcl cannot handle     //
# //                 long file names properly
# ////////////////////////////////////////////////////////////////////////////////

import sys
import math

class MemoryArea(object):
    def __init__(self):
        self.size = 0
        self.base = None

    def regWord(self, addr):
        if self.base == None:
            self.base = addr
            self.size += 4
            return True
        elif addr == self.base + self.size:
            self.size += 4
            return True
        elif addr == self.base - 4:
            self.base -= 4
            self.size += 4
            return True
        return False


class Stim_file(object):
    def __init__(self, path, base, size):
        self.path = path
        self.file = None
        self.base = base
        self.size = size

    def getFile(self):
        if self.file == None and self.path != None:
            self.file = open(self.path, 'w')
        return self.file

    def regWord(self, addr, data):
        if addr >= self.base and addr < self.base + self.size:
            if self.getFile() != None:
                self.getFile().write("@%08X %s\n" % ((addr - self.base) >> 2, data))
            return True

        return False

    def close(self):
        if self.file != None:
            self.file.close()

class FlashStimFile(object):
    def __init__(self, path, archi=None, core=None):
        self.file = open(path, 'w')
        self.blockSize = 4096
        self.nbAreas = 0
        if archi == 'patronus' and core == 'secure':
            self.entry = 0x1c000100
        else:
            self.entry = 0x1c000080
        self.bootaddr = 0x1c000000

    def close(self):
        self.file.close()

    def writesArea(self, areas):
        flashOffset = 0

        print ("Generating flash stimuli")
        print ("  Nb areas: %d" % len(areas))

        # First compute areas flash information
        flashOffset = 4 + 4 + 4 + len(areas) * 4 * 4

        index = 0
        for area in areas:
            area.nbBlocks = (area.size + self.blockSize - 1) / self.blockSize
            area.offset = flashOffset
            flashOffset += area.nbBlocks * 4096
            print ("  Area %d: offset: 0x%x, base: 0x%x, size: 0x%x, nbBlocks: %d" % (index, area.offset, area.base, area.size, area.nbBlocks))
            index += 1


        # Then write the header containing memory areas declaration
        flashOffset = 0

        flashOffset += dump_bytes(self.file, flashOffset, "%08X"%len(areas))
        flashOffset += dump_bytes(self.file, flashOffset, "%08X"%self.entry)
        flashOffset += dump_bytes(self.file, flashOffset, "%08X"%self.bootaddr)

        for area in areas:
            flashOffset += dump_bytes(self.file, flashOffset, "%08X"%area.offset)
            flashOffset += dump_bytes(self.file, flashOffset, "%08X"%area.base)
            flashOffset += dump_bytes(self.file, flashOffset, "%08X"%area.size)
            flashOffset += dump_bytes(self.file, flashOffset, "%08X"%area.nbBlocks)

        # Finally write the data
        for area in areas:
            flashOffset = area.offset
            for addr in range(area.base, area.base + area.size, 4):
                flashOffset += dump_bytes(self.file, flashOffset, slm_dict[addr>>2])


class Stimuli(object):
    def __init__(self):
        self.stimFileRanges = []
        self.areas = []
        self.flashFile = None

    def regFile(self, path, base, size):
        file = Stim_file(path, base, size)
        if base != None:
            self.stimFileRanges.append(file)

    def regFlashFile(self, path, archi=None, core=None):
        self.flashFile = FlashStimFile(path, archi, core)

    def regWord(self, addr, data):

        found = False
        for area in self.areas:
            if area.regWord(addr):
                found = True
                break
        if not found:
            area = MemoryArea()
            area.regWord(addr)
            self.areas.append(area)

        for stimFile in self.stimFileRanges:
            if stimFile.regWord(addr, data):
                return True
        return False

    def writeFlash(self):
        self.flashFile.writesArea(self.areas)

    def close(self):
        if self.flashFile != None: self.flashFile.close()
        for stimFile in self.stimFileRanges:
            stimFile.close()

stimuli = Stimuli()


###############################################################################
# Function to dump single bytes of a string to a file
###############################################################################
def dump_bytes( filetoprint, addr, data_s):
    for i in xrange(0,4,1):
        filetoprint.write("@%08X %s\n" % ( addr+i,  data_s[(3-i)*2:((3-i)+1)*2] ))
    return 4

###############################################################################
# Read s19 file and put data bytes into a dictionary
###############################################################################
def s19_parse(filename, s19_dict):
    s19_file = open(filename, 'r')
    for line in s19_file:
        rec_field = line[:2]
        prefix    = line[:4]

        if rec_field == "S0" or prefix == "S009" or prefix == "S505" or prefix == "S705" or prefix == "S017" or prefix == "S804" or line == "":
            continue

        data = line[-6:-4] # extract data byte
        str_addr = line[4:-6]

        addr = int("0x%s" % str_addr, 0)

        s19_dict[addr] = data

    s19_file.close()

###############################################################################
# arrange bytes in words
###############################################################################
def bytes_to_words(byte_dict, word_dict):
    for addr in byte_dict:
        wordaddr = addr >> 2
        data = "00000000"

        if wordaddr in word_dict:
            data = word_dict[wordaddr]

        byte = addr % 4
        byte0 = data[0:2]
        byte1 = data[2:4]
        byte2 = data[4:6]
        byte3 = data[6:8]
        new   = byte_dict[addr]

        if byte == 0:
            data = "%s%s%s%s" % (byte0, byte1, byte2, new)
        elif byte == 1:
            data = "%s%s%s%s" % (byte0, byte1, new,   byte3)
        elif byte == 2:
            data = "%s%s%s%s" % (byte0, new,   byte2, byte3)
        elif byte == 3:
            data = "%s%s%s%s" % (new,   byte1, byte2, byte3)

        word_dict[wordaddr] = data

###############################################################################
# Start of file
###############################################################################
if(len(sys.argv) < 2):
    print "Usage s19toslm.py FILENAME"
    quit()

archi = None
core = None
if len(sys.argv) > 2: archi = sys.argv[2]
if len(sys.argv) > 3: core = sys.argv[3]

###############################################################################
# Parse s19 file
###############################################################################
s19_dict = {}
slm_dict = {}

s19_parse(sys.argv[1], s19_dict)

bytes_to_words(s19_dict, slm_dict)

###############################################################################
# open files
###############################################################################

stimuli.regFile("tcdm.slm",    0x10000000, 128 * 1024)
stimuli.regFile("fcTcdm.slm",  0x1B000000, 64 * 1024)
stimuli.regFile("l2_stim.slm", 0x1C000000, 512 * 1024)
stimuli.regFile(None         , 0x1A000000, 64 * 1024)

if archi != 'GAP':
    stimuli.regFlashFile("flash_stim.slm", archi, core)

###############################################################################
# write the stimuli
###############################################################################
for addr in sorted(slm_dict.keys()):
    fullAddr = addr << 2
    if not stimuli.regWord(fullAddr, slm_dict[addr]): 
        raise Exception("Got s19 address from unknown memory range: 0x%x" % fullAddr)

###############################################################################
# write flash
###############################################################################
if archi != 'GAP':
    stimuli.writeFlash()

###############################################################################
# close all files
###############################################################################
stimuli.close()
