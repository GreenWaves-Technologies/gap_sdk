#!/bin/bash

### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

CURRENT_PATH=`pwd`

BINARY=$CURRENT_PATH/BUILD/GAP8_V2/GCC_RISCV/test
FLASH_IMG=$CURRENT_PATH/flashImg.raw

JTAG_TCL=$GAP_SDK_HOME/tools/gap8-openocd-tools/tcl/jtag_boot.tcl
FLASH_TCL=$GAP_SDK_HOME/tools/gap8-openocd-tools/tcl/flash_image.tcl
FUSER_TCL=$GAP_SDK_HOME/tools/gap8-openocd-tools/tcl/fuser.tcl
FUSER_FLASH_TCL=$GAP_SDK_HOME/tools/gap8-openocd-tools/tcl/fuser_flash.tcl

GAP_FLASHER_GAPOC=$GAP_SDK_HOME/tools/gap8-openocd-tools/gap_bins/gap_flasher@gapoc_a.elf
GAP_FUSER_GAPOC=$GAP_SDK_HOME/tools/gap8-openocd-tools/gap_bins/gap_fuser@gapoc_a.elf

#flashImageBuilder --verbose --flash-boot-binary=$BINARY --raw=$FLASH_IMG --comp=$CURRENT_PATH/BUILD_MODEL_16BIT/lynred_L3_Flash_Const.dat --comp=$CURRENT_PATH/Calibration.bin

FILESIZE=$(stat -c%s "$FLASH_IMG")
echo "File Size is: $FILESIZE"

#Open openocd with proper commands
openocd -f interface/ftdi/olimex-arm-usb-ocd-h.cfg -f target/gap8revb.tcl -f $JTAG_TCL -f $FLASH_TCL > /dev/null &

TO_KILL_1=$!

sleep 3

(
	echo "poll off"
	sleep 1
	echo "irscan gap8.cpu 0x7"
	sleep 1
	echo "drscan gap8.cpu 0x4 0x0"
	sleep 1
	echo "jtag_reset 0 1"
	sleep 1
	echo "jtag_reset 0 0"
	sleep 1
	echo "exit"
) | telnet localhost 4444


sleep 2

kill $TO_KILL_1

