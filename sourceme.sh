#! /bin/bash

if [ -n "${ZSH_VERSION:-}" ]
then
    DIR="$(readlink -f -- "${(%):-%N}")"
    DIRNAME="$(dirname $DIR)"
    GAP_SDK_HOME=$(echo $DIRNAME)
    export GAP_SDK_HOME
else
    export GAP_SDK_HOME="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
fi

EXCLUDE="ai_deck.sh common.sh gap8.sh gap8_v2.sh gapuino_v3.sh"
EXCLUDE_FILES=
for i in $EXCLUDE
do
    EXCLUDE_FILES="$EXCLUDE_FILES -not -iname *$i"
done

BOARDS=$(find $GAP_SDK_HOME/configs -maxdepth 1 -type f -iname "*.sh" $EXCLUDE_FILES | sort)

n=0
board=
for i in $BOARDS
do
    n=$(($n+1))
    board=$(basename $(echo $BOARDS | cut -d ' ' -f $n))
    echo -e "\t$n - $board"
done

read board

if [ -z "$board" ] || [ "$board" -gt "$n" ] || [ "$board" -le 0 ]
then
    source $GAP_SDK_HOME/configs/gapuino.sh
else
    board=$(echo $BOARDS | cut -d ' ' -f $board)
    source $board
fi

echo The target board you have chosen is : $BOARD_NAME, $TARGET_CHIP.

unset EXCLUDE EXCLUDE_FILES BOARDS board n
