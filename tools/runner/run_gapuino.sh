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
echo "---------------------------------GAPUINO------------------------------------"
echo ""

here=`pwd`
cd $here/BUILD/GAP8/GCC_RISCV

FLAG_FLASH=0
# A flag for choosing the boot mode: 0-Noboot, 1-jtag, 2-hyper
FLAG_BOOT=0

#Parameters of bridge
verbose=0
cable=ftdi@digilent
boot_mode=jtag
gdb=""
ioloop=ioloop
reqloop=reqloop
binary="--binary=test"

buildFlashImage=""

if [ "$#" -eq 0 ]
then
    plpbridge --verbose=$verbose --cable=$cable --boot-mode=$boot_mode $binary --chip=gap load $ioloop $reqloop start wait
else
    while [ "$#" -gt 0 ]
    do
        case ${1} in
            #Mode using the script
            -s) script_filename="$(echo $2 | cut -d'@' -f2 )"
                cp $here/$script_filename .
                plpbridge  --verbose=$verbose --cable=$cable --boot-mode=$boot_mode --binary=test --chip=gap --script=$2 load $ioloop $reqloop start script wait
                exit
                ;;
            #Mode using gdb
            -gdb)
                gdb=gdb
                ;;
            #Mode using flasher
            -f)
                FLAG_FLASH=1
                buildFlashImage="flashImageBuilder --verbose --flash-boot-binary=test --raw=flashImg.raw"
                ;;
            -ftdi)
                cable=ftdi
                ;;
            -jtag)
                boot_mode=jtag
                ;;
            -hyper)
                ioloop=""
                reqloop=""
                binary=""
                boot_mode=jtag_hyper
                ;;
            *)
                if [ "$FLAG_FLASH" -eq "1" ]
                then
                    buildFlashImage="$buildFlashImage --comp=$here/$1"
                fi
                ;;
        esac
        shift
    done

    # Now we can produce the flash image, flash the file and reboot if necessary
    if [ "$FLAG_FLASH" -eq "1" ]
    then
        eval $buildFlashImage
        plpbridge  --verbose=$verbose --cable=$cable --boot-mode=jtag --flash-image=flashImg.raw --chip=gap flash wait
    fi
    plpbridge --verbose=$verbose --cable=$cable --boot-mode=$boot_mode $binary --chip=gap load $ioloop $reqloop start $gdb wait

fi

