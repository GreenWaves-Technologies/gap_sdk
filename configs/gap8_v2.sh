#! /bin/bash

export GAP_SDK_HOME="$(dirname $(dirname "$(readlink -f "${BASH_SOURCE[0]}")"))"

export TARGET_CHIP="GAP8_V2"
export TARGET_NAME="gap_rev1"
export PULP_CURRENT_CONFIG=gap_rev1@config_file=chips/gap_rev1/gap_rev1.json
if [ -z "$BOARD_NAME" ]; then
    export BOARD_NAME="gapuino"
fi
export GVSOC_CONFIG=$BOARD_NAME

source $GAP_SDK_HOME/configs/common.sh