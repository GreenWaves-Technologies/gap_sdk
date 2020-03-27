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

export TARGET_CHIP_FAMILY="GAP8"
export TARGET_CHIP="GAP8_V3"
export TARGET_NAME="gap8_revc"
export BOARD_NAME=gapuino
export PULP_CURRENT_CONFIG=$BOARD_NAME@config_file=config/gapuino_revc.json
export GVSOC_CONFIG=gapuino_revc
export PLPBRIDGE_CABLE=ftdi@digilent

export GAPY_TARGET=gapuino_v3

source $GAP_SDK_HOME/configs/common.sh