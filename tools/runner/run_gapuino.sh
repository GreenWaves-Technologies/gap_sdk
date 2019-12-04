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
cd $here/BUILD/$TARGET_CHIP/GCC_RISCV

FLAG_FLASH=0
FLAG_SCRIPT=0
# A flag for choosing the boot mode: 0-Noboot, 1-jtag, 2-hyper
FLAG_BOOT=0
FLAG_NORUN=0
FLAG_NOFLASH=0

#Parameters of bridge
verbose=2
fileio=0
if [ "$BOARD_NAME" = "gapoc_a" ];
then
    cable=ftdi
else
    cable=ftdi@digilent
fi
boot_mode=jtag
gdb=""
ioloop=ioloop
reqloop=reqloop
binary="--binary=test"

buildFlashImage=""

if [ "$#" -eq 0 ]
then
    if [ -n "$GAP_USE_OPENOCD" ]; then
        openocd -f $OPENOCD_CABLE -f $OPENOCD_CHIP_TARGET -f tcl/jtag_boot.tcl -c 'gap8_jtag_load_binary_and_start test elf'
    else
        plpbridge --verbose=$verbose --cable=$cable --boot-mode=$boot_mode $binary --chip=$TARGET_NAME load $ioloop $reqloop start wait &
    fi
    pid_bridge=$!
else
    while [ "$#" -gt 0 ]
    do
        case ${1} in
            #Mode using the script
            -s) script_filename="$(echo $2 | cut -d'@' -f2 )"
                cp $here/$script_filename .
                FLAG_SCRIPT=1
                plpbridge  --verbose=$verbose --cable=$cable --boot-mode=$boot_mode --binary=test --chip=$TARGET_NAME load $ioloop $reqloop start script --script=$2
                ;;
            #Mode using gdb
            -gdb)
                gdb=gdb
                ;;
            #Mode using flasher
            -norun)
                FLAG_NORUN=1
                ;;
            -noflash)
                FLAG_NOFLASH=1
                ;;
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
            -fileIO)
                if [ "$#" -gt 1 ]
                then
                    shift
                    fileio=$1
                else
                    fileio=2
                fi

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
    if [ -n "$GAP_USE_OPENOCD" ]; then
        if [ "$FLAG_FLASH" -eq "1" ]
        then
            if [ "$FLAG_NOFLASH" -eq "0" ]; then
                eval $buildFlashImage
                openocd-flasher flashImg.raw
            fi
            if [ "$FLAG_NORUN" -eq "0" ]; then
                openocd -f $OPENOCD_CABLE -f $OPENOCD_CHIP_TARGET -f tcl/jtag_boot.tcl -c 'gap8_jtag_load_binary_and_start test elf'
            fi
        fi
    else
        if [ "$FLAG_FLASH" -eq "1" ]
        then
            eval $buildFlashImage
            plpbridge  --verbose=$verbose --cable=$cable --boot-mode=jtag --flash-image=flashImg.raw --chip=$TARGET_NAME flash wait
        fi
        if [ "$FLAG_SCRIPT" -eq "0" ]
        then
            plpbridge --verbose=$verbose --cable=$cable --boot-mode=$boot_mode $binary --chip=$TARGET_NAME load $ioloop $reqloop start $gdb wait $script &
            pid_bridge=$!
        fi
    fi
fi

if [ -z "$GAP_USE_OPENOCD" ]; then
    if [ "$fileio" -gt 0 ]
    then
        sleep $fileio
        plpbridge-rt --verbose=$verbose $binary --chip=$TARGET_NAME&
    fi

    if [ "$FLAG_SCRIPT" -eq "0" ]
    then
       wait $pid_bridge
    fi
fi
