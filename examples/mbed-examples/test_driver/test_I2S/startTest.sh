#! /bin/bash

make clean all

path=$(pwd)
cp i2s.py "$path/BUILD/GAP8/GCC_RISCV"

cd "$path/BUILD/GAP8/GCC_RISCV"

date
plpbridge --cable=ftdi@digilent --boot-mode=jtag --chip=gap --binary=test load ioloop start script --script=i2s@i2s.py
date

mv wavOut.wav ../../..
