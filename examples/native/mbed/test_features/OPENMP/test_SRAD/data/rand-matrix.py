#!/usr/bin/python

import numpy as np
import sys
import glob, csv, xlwt, os
import subprocess


size     = 64
filename = "input-matrix-%s.h" % (size)

if(len(sys.argv)>2):
    size = int(sys.argv[1])
    filename = sys.argv[2]
else:
    if(len(sys.argv)>1):
        size = int(sys.argv[1])
        filename = "input-matrix-%s.h" % (size)

inputMatrix = np.random.random_sample((size*size))
inputList   = inputMatrix.tolist()

f = open(filename,'w')

#Write header
f.write("#ifndef __INPUT_MATRIX_H__\n")
f.write("#define __INPUT_MATRIX_H__\n\n")

#f.write("#ifdef X86\n")
f.write("float J[%s*%s] = {\n" % (size,size))
#f.write("#else\n")
#f.write("__attribute__((section(.heapl2ram))) float J[%s*%s] = {\n" % (size,size))
#f.write("#endif\n")

for y in range(0,size):
    row = ""
    new = "%.5f" % inputList[y*size]
    row += new
    for x in range(1,size):
        new = ",\t%.5f" % inputList[y*size+x]
        row += new
    
    if y != size-1:
        row += ","
    
    row += "\n"
    f.write(row)

f.write("};\n")
f.write("#endif //__INPUT_MATRIX_H__\n\n");
f.close()

print "Generated Random Input Matrix: %s" % (filename)
