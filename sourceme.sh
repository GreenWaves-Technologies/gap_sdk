#! /bin/bash

export GAP_SDK_HOME="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

read -p "Which chip you want to use: 1-GAP8 2-GAP8_V2 [1]: " chip
chip=${chip:-1}
if [ $chip == "1"  ]; then
    source $GAP_SDK_HOME/configs/gap8.sh
elif [ $chip == "2"  ]; then
    source $GAP_SDK_HOME/configs/gap8_v2.sh
fi
echo The target chip you have chosen is : $TARGET_CHIP
