#! /bin/bash

export CUSTOM_BSP_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# For JLINK Debugger
#export GAPY_OPENOCD_CABLE=interface/jlink.cfg
#export OPENOCD_CABLE=interface/jlink.cfg

# For OLIMEX Debugger
export GAPY_OPENOCD_CABLE=interface/ftdi/olimex-arm-usb-ocd-h.cfg
export OPENOCD_CABLE=interface/ftdi/olimex-arm-usb-ocd-h.cfg

export BOARD_NAME=gapoc_c
