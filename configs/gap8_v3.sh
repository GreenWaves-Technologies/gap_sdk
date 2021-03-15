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
export TARGET_CHIP="GAP8_V3"
export TARGET_NAME="gap8_revc"
export PULP_CURRENT_CONFIG=gap8_revc@config_file=chips/gap8_revc/gap8_revc.json
if [ -z "$BOARD_NAME" ]; then
    export BOARD_NAME="gapuino"
fi
export GVSOC_CONFIG=$BOARD_NAME
export PLPBRIDGE_CABLE=ftdi@digilent

export GAPY_TARGET=gapuino_v3

source $GAP_SDK_HOME/configs/common.sh
