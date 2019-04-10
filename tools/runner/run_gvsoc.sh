#!/bin/sh
echo ""
echo ""
echo  "\033[32m          ,ad8888ba,        db        88888888ba  \033[0m ad88888ba   "
echo  "\033[32m         d8\"'    \`\"8b      d88b       88      \"8b\033[0m d8\"     \"8b  "
echo  "\033[32m        d8'               d8'\`8b      88      ,8P\033[0m Y8a     a8P  "
echo  "\033[32m        88               d8'  \`8b     88aaaaaa8P' \033[0m \"Y8aaa8P\"   "
echo  "\033[32m        88      88888   d8YaaaaY8b    88\"\"\"\"\"\"'  \033[0m  ,d8\"\"\"8b,   "
echo  "\033[32m        Y8,        88  d8\"\"\"\"\"\"\"\"8b   88       \033[0m   d8\"     \"8b  "
echo  "\033[32m         Y8a.    .a88 d8'        \`8b  88        \033[0m  Y8a     a8P  "
echo  "\033[32m          \`\"Y88888P\" d8'          \`8b 88         \033[0m  \"Y88888P\"   \033[0m"

echo ""

echo ""
echo "---------------------------------GVSOC_SIMULATION------------------------------------"
echo ""

vcd=""
vcd_en=0
trace=""
trace_option=""
usr_config=""

#pulp-run --platform=gvsoc --config=gap_rev1 --config-user=/home/yao/Projects/SDK/gap_sdk_master/examples/rt-tests/quick/periph/camera/simple/config.ini --binary=test prepare run


cd `pwd`/BUILD/$TARGET_CHIP/GCC_RISCV/
run_gvsoc="pulp-run --platform=gvsoc --config=$TARGET_NAME --binary=test"
while [ "$#" -gt 0 ]
do
    case ${1} in
        -vcd)
            vcd_en=1
            vcd=" --vcd --event-tag=debug --event=.*"
            ;;
        -trace)
            shift
            if [ ${1} = all ]
            then
                trace_option=".*"
            else
                trace_option=$1
            fi
            trace=" --trace=$trace_option"
            ;;
        -config)
            shift
            usr_config=" --config-user=$1"
            ;;
        *)
            echo "Wrong parameter! Please using only these parameters:"
            echo " Trace: trace=<the trace you want>"
            echo " vcd: vcd=1"
            echo " For example:"
            echo "       make clean all run platform=gvsoc trace=insn vcd=1"
            ;;

    esac
    shift
done

run_gvsoc="$run_gvsoc $usr_config $trace $vcd prepare run"
echo $run_gvsoc
eval $run_gvsoc

if test $vcd_en -eq 1
then
gtkwave view.gtkw
fi

cd -

