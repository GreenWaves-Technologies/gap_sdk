#! /bin/bash

if [  -n "${ZSH_VERSION:-}" ]; then 
	DIR="$(readlink -f -- "${(%):-%N}")"
	DIRNAME="$(dirname $DIR)"
	GAP_SDK_HOME=$(echo $DIRNAME)
	export GAP_SDK_HOME
else
	export GAP_SDK_HOME="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
fi

echo "Which board you want to use:
     1-GAPUINO
     2-GAPUINO_V2
     3-GAPOC_A
     4-GAPOC_A_V2
     5-GAPOC_B
     6-GAPOC_B_V2 [1]: "
read chip
chip=${chip:-1}
if [ $chip -eq 1  ]; then
    source $GAP_SDK_HOME/configs/gapuino.sh
elif [ $chip -eq 2  ]; then
    source $GAP_SDK_HOME/configs/gapuino_v2.sh
elif [ $chip -eq 3  ]; then
    source $GAP_SDK_HOME/configs/gapoc_a.sh
elif [ $chip -eq 4  ]; then
    source $GAP_SDK_HOME/configs/gapoc_a_v2.sh
elif [ $chip -eq 5  ]; then
    source $GAP_SDK_HOME/configs/gapoc_b.sh
elif [ $chip -eq 6  ]; then
    source $GAP_SDK_HOME/configs/gapoc_b_v2.sh
else
	echo "Unknown board: $chip"
fi
echo The target board you have chosen is : $BOARD_NAME
