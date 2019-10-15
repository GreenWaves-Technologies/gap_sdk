#! /bin/bash

export GAP_SDK_HOME="$(dirname $(dirname "$(readlink -f "${BASH_SOURCE[0]}")"))"

export TARGET_CHIP="GAP8_V2"
export TARGET_NAME="gap_rev1"
export BOARD_NAME=gapuino
export PULP_CURRENT_CONFIG=$BOARD_NAME@config_file=config/$BOARD_NAME.json
export GVSOC_CONFIG=$BOARD_NAME

source $GAP_SDK_HOME/configs/common.sh