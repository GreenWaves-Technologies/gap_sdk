#!/bin/bash -ex

export GAP_SDK_HOME="$(dirname $(dirname "$(readlink -f "${BASH_SOURCE[0]}")"))"

export TARGET_CHIP="GAP8"
export TARGET_NAME="gap"
export PULP_CURRENT_CONFIG=gap@config_file=chips/gap/gap.json
if [ -z "$BOARD_NAME" ]; then
    export BOARD_NAME="gapuino"
fi
export GVSOC_CONFIG=$BOARD_NAME

. $GAP_SDK_HOME/configs/common.sh