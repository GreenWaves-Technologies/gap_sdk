#! /bin/bash


if [  -n "${ZSH_VERSION:-}" ]; then 
	DIR="$(readlink -f -- "${(%):-%x}")"
	DIRNAME="$(dirname $DIR)"
	GAP_SDK_HOME=$(dirname $DIRNAME)
	export GAP_SDK_HOME
	#echo $(dirname "$(readlink -f ${(%):-%N})")
else
	export GAP_SDK_HOME="$(dirname $(dirname "$(readlink -f "${BASH_SOURCE[0]}")"))"
fi

source $GAP_SDK_HOME/configs/openocd-gap8.sh

export TARGET_CHIP_FAMILY="GAP8"
export TARGET_CHIP="GAP8_V2"
export TARGET_NAME="gap_rev1"
export BOARD_NAME=ai_deck
export PULP_CURRENT_CONFIG=$BOARD_NAME@config_file=config/ai_deck.json
export GVSOC_CONFIG=ai_deck
export OPENOCD_CHIP_TARGET=target/gap8revb.tcl
export OPENOCD_CABLE=interface/ftdi/olimex-arm-usb-ocd-h.cfg
export PLPBRIDGE_CABLE=ftdi

export GAPY_TARGET=ai_deck

source $GAP_SDK_HOME/configs/common.sh
