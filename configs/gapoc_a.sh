#! /bin/bash

export GAP_SDK_HOME="$(dirname $(dirname "$(readlink -f "${BASH_SOURCE[0]}")"))"

export TARGET_CHIP_FAMILY="GAP8"
export TARGET_CHIP="GAP8"
export TARGET_NAME="gap"
export BOARD_NAME=gapoc_a
export PULP_CURRENT_CONFIG=$BOARD_NAME@config_file=config/$BOARD_NAME.json
export GVSOC_CONFIG=$BOARD_NAME

source $GAP_SDK_HOME/configs/common.sh