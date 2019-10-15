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
# // Description:    s19 to header conversion tool for Wolf SoC compilation     //
# //                                                                            //
# // Revision:                                                                  //
# // Revision v0.1 - File Created                                               //
# // Revision v0.2 - Modification: Compiler does now generate little endian     //
# //                 directly. revert bytes!                                    //
# // Revision v0.3 - Moved from 128 bit s19 to 8 bit s19 file. This solves our  //
# //                 problems with misaligned addresses in s19 files.           //
# // Revision v0.4 - Added TCDM memory initialization                           //
# // Revision v0.5 - Rewrote the whole thing in python as tcl cannot handle     //
# //                 long file names properly                                   //
# // Revision v0.5 - created the header target                                  //
# ////////////////////////////////////////////////////////////////////////////////

import sys
import math

###############################################################################
# Function to dump single bytes of a string to a file
###############################################################################
def dump_bytes( filetoprint, addr, data_s):
    for i in range(0,4,1):
        filetoprint.write("@%08X %s\n" % ( addr+i,  data_s[i*2:(i+1)*2] ))

###############################################################################
# Start of file
###############################################################################
if(len(sys.argv) < 2):
    print("Usage s19toheader.py FILENAME")
    quit()

l2_bank_size = 131072 # in words (32 bit)
l2_start     = 0x1C000000

###############################################################################
# Parse s19 file
###############################################################################
s19_file = open(sys.argv[1], 'r')
s19_dict = {}

for line in s19_file:
    rec_field = line[:2]
    prefix    = line[:4]

    if rec_field == "S0" or prefix == "S009" or prefix == "S505" or prefix == "S705" or prefix == "S017" or line == "":
        continue

    addr = int("0x%s" % line[4:12], 0)
    data = line[12:14]

    s19_dict[addr] = data

s19_file.close()

slm_dict = {}

for addr in s19_dict:
    wordaddr = addr >> 2
    data = "00000000"

    if wordaddr in slm_dict:
        data = slm_dict[wordaddr]

    byte = addr % 4
    byte0 = data[0:2]
    byte1 = data[2:4]
    byte2 = data[4:6]
    byte3 = data[6:8]
    new   = s19_dict[addr]

    if byte == 0:
        data = "%s%s%s%s" % (byte0, byte1, byte2, new)
    elif byte == 1:
        data = "%s%s%s%s" % (byte0, byte1, new,   byte3)
    elif byte == 2:
        data = "%s%s%s%s" % (byte0, new,   byte2, byte3)
    elif byte == 3:
        data = "%s%s%s%s" % (new,   byte1, byte2, byte3)

    slm_dict[wordaddr] = data

# word align all addresses
l2_start   = l2_start   >> 2

###############################################################################
# open files
###############################################################################
#l2_data      = open("l2_data.h",    'w')
l2_sections  = open("l2_sections.h",'w')

###############################################################################
# write the stimuli
###############################################################################

prev_addr     = l2_start
section_index = 0
section_size  = []
section_addr  = []
section_data  = []
size          = 0
init          = 0


# WRITE NON COMPRESSED HEADER FILE
#l2_data.write("int l2_size = %d;\n\n" % len(slm_dict))

#l2_data.write("int l2_data[%d] = {\n" % len(slm_dict)*2)

for curr_addr in sorted(slm_dict.keys()):
    if init==0:
        section_addr.append((curr_addr<<2))
        init = init+1
    data = slm_dict[curr_addr]
#    l2_data.write("0x%08X, 0x%s,\n" % ((curr_addr<<2), data))
    section_data.append(data)
    if (prev_addr+1 != curr_addr):
        if section_index==0:
            section_size.append(size)
        else:
            section_size.append(size+1)
        section_addr.append((curr_addr<<2))
        section_index = section_index+1
        size = 0
    else:
        size = size+1
    prev_addr = curr_addr

section_addr.append((curr_addr<<2))
section_size.append(size+1)
section_index = section_index+1

#l2_data.write("};\n\n")

# WRITE COMPRESSED HEADER FILE
l2_sections.write("int section_num = %d;\n\n" % section_index)

l2_sections.write("int section_addr[%d] = { " % section_index)

for i in range(0,section_index):
    l2_sections.write("0x%8x, " % section_addr[i])

l2_sections.write("}; \n\n")

l2_sections.write("int section_size[%d] = { " % section_index)

for i in range(0,section_index):
    l2_sections.write("%d, " % section_size[i])

l2_sections.write("}; \n\n")

l2_sections.write("int section_data[%d] = { \n" % len(section_data))

for i in range(len(section_data)):
    l2_sections.write("0x%s,\n" % section_data[i])
    
l2_sections.write("}; \n\n")

###############################################################################
# close all files
###############################################################################
#l2_data.close()
l2_sections.close()
