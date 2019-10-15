#!/usr/bin/python
import os
import sys
import time
from PIL import Image

def cutFile(inName):
    sizeMax = 350*1024
    fileSize = os.path.getsize(inName)
    nFiles = fileSize/sizeMax + 1
    pathOut = "./fileCut"
    try:
        os.stat(pathOut)
    except:
        os.mkdir(pathOut)
    for i in range(nFiles):
        if i < (nFiles-1):
            os.popen("dd skip="+ str(i*sizeMax) +" count="+ str(sizeMax) +" if="+ str(inName) +" of="+ pathOut +"/flashImgCut" + str(i) + " bs=1")
            print "dd skip="+ str(i*sizeMax) +" count="+ str(sizeMax) +" if="+ str(inName) +" of="+ pathOut +"/flashImgCut" + str(i) + " bs=1"
        else:
            os.popen("dd skip="+ str(i*sizeMax) +" if="+ str(inName) +" of="+ pathOut +"/flashImgCut" + str(i) + " bs=1")
            print "dd skip="+ str(i*sizeMax) +" if="+ str(inName) +" of="+ pathOut +"/flashImgCut" + str(i) + " bs=1"
    return pathOut


def main():
    fileName = str(sys.argv[1])
    print fileName
    pathFiles = cutFile(fileName)
    imgFiles = os.listdir(pathFiles)
    nm_path = os.environ['PULP_RISCV_GCC_TOOLCHAIN_CI'] + '/bin/'
    print nm_path + "riscv32-unknown-elf-nm build/template.gap_platform.fpga/test/test |grep -w 'flashCopyHeader$'"

#####################################
#       Load the file
#####################################
    addrImgInHead = os.popen(nm_path + "riscv32-unknown-elf-nm build/template.gap_platform.fpga/test/test |grep -w 'flashCopyHeader$'").read()
    addrImgInHead = (addrImgInHead.split())[0]
    addrImgIn = os.popen(nm_path + "riscv32-unknown-elf-nm build/template.gap_platform.fpga/test/test |grep -w 'L2_Buffer$'").read()
    addrImgIn = (addrImgIn.split())[0]

    start_time = time.time()
    imgFiles = sorted(imgFiles)
    for imgFile in imgFiles:
        file2Bsent = pathFiles + "/" + imgFile
        fileSize = os.path.getsize(file2Bsent)
        print file2Bsent
        print fileSize

        ImgOutFlag = os.popen("debug_bridge -c proxy -b gap -rm -m " + hex(int(("0x"+addrImgInHead), 16) + 8)).read()
        while ( int(ImgOutFlag) == 1 ):
            time.sleep(1)
            ImgOutFlag = os.popen("debug_bridge -c proxy -b gap -rm -m " + hex(int(("0x"+addrImgInHead), 16) + 8)).read()

        os.popen("debug_bridge -c proxy -b gap -wf " + file2Bsent + " -m " + hex(int(("0x"+addrImgIn), 16)) + " -size " + str(fileSize))
        os.popen("debug_bridge -c proxy -b gap -wm " + str(fileSize) + " -m " + hex(int(("0x"+addrImgInHead), 16) + 4))
        os.popen("debug_bridge -c proxy -b gap -wm 1 -m " + hex(int(("0x"+addrImgInHead), 16) + 8))
        print "upload image (" + str(fileSize) + ") spent " + str(time.time() - start_time) + " Sec."

        ImgOutFlag = os.popen("debug_bridge -c proxy -b gap -rm -m " + hex(int(("0x"+addrImgInHead), 16) + 16)).read()
        while ( int(ImgOutFlag) == 0 ):
            time.sleep(1)
            ImgOutFlag = os.popen("debug_bridge -c proxy -b gap -rm -m " + hex(int(("0x"+addrImgInHead), 16) + 16)).read()
        os.popen("debug_bridge -c proxy -b gap -wm 0 -m " + hex(int(("0x"+addrImgInHead), 16) + 16))

    os.popen("debug_bridge -c proxy -b gap -wm 1 -m " + hex(int(("0x"+addrImgInHead), 16) + 12))

if __name__ == '__main__':
    main()
