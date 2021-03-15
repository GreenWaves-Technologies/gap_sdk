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
export BOARD_NAME=gapuino
export PULP_CURRENT_CONFIG=$BOARD_NAME@config_file=config/gapuino_revb_spi.json
export GVSOC_CONFIG=gapuino_revb_spi
export OPENOCD_CHIP_TARGET=target/gap8revb.tcl
export OPENOCD_CABLE=interface/ftdi/gapuino_ftdi.cfg
export PLPBRIDGE_CABLE=ftdi@digilent

export PULPOS_BOARD=gapuino
export PULPOS_BOARD_VERSION=gapuino
export PULPOS_BOARD_PROFILE=gapuino
export PULPOS_TARGET=gap8
export PULPOS_SYSTEM=gapuino
export GAPY_TARGET=gapuino_v2_spi


source $GAP_SDK_HOME/configs/common.sh
