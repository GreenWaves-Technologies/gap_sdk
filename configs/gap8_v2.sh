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
export PULP_CURRENT_CONFIG=gap_rev1@config_file=chips/gap_rev1/gap_rev1.json
if [ -z "$BOARD_NAME" ]; then
    export BOARD_NAME="gapuino"
fi
export GVSOC_CONFIG=$BOARD_NAME
export PLPBRIDGE_CABLE=ftdi@digilent

export GAPY_TARGET=gapuino_v2

source $GAP_SDK_HOME/configs/common.sh