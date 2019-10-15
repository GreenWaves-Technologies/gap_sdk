#!/bin/sh

echo ""
echo "---------------------------------FPGA_SIMULATION------------------------------------"
echo ""
cd `pwd`/BUILD/GAP8/GCC_RISCV

riscv32-unknown-elf-objcopy -j .heapsram -O binary test test.tcdm.bin

riscv32-unknown-elf-objcopy -j .vectors -j .text -j .data -j .ram -j .dtors -j .rodata -j .got -j .boot -j .heapl2ram -O binary test test.l2.bin

debug_bridge --proxy -c shell -b gap -copt "ssh $PULP_EMU_ADDR sudo $PULP_EMU_WWOLF -p -l" --binary test --load -m 0x1A104074 -wm 0x00000001 --printf --start -1 --loop
