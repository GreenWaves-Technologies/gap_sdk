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
buildFlashImage=""

if [ "$#" -eq 0 ]
then
    plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start wait
else
    while [ "$#" -gt 0 ]
    do
        case ${1} in
            -s) script_filename="$(echo $2 | cut -d'@' -f2 )"
                cp $here/$script_filename .
                plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start script --script=$2
                ;;
            -gdb)
                plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop start gdb wait
                ;;
            -f)
                FLAG_FLASH=1
                buildFlashImage="flashImageBuilder --verbose --flash-boot-binary=test --raw=flashImg.raw"
                ;;
            -jtag)
                FLAG_BOOT=1
                ;;
            -hyper)
                FLAG_BOOT=2
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
fi

# Now we can produce the flash image, flash the file and reboot if necessary
if [ "$FLAG_FLASH" -eq "1" ]
then
    eval $buildFlashImage
    # Flash the flash image and boot from jtag
    if [ "$FLAG_BOOT" -eq "1" ]
    then
        plpbridge --cable=ftdi@digilent --boot-mode=jtag --flash-image=flashImg.raw --chip=gap flash wait
        plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start wait
    # Flash the flash image and boot from hyper
    elif [ "$FLAG_BOOT" -eq "2" ]
    then
        plpbridge --cable=ftdi@digilent --boot-mode=jtag --flash-image=flashImg.raw --chip=gap flash wait
        plpbridge --cable=ftdi@digilent --boot-mode=jtag_hyper --chip=gap load start wait
    # Flash the flash image only
    else
        plpbridge --cable=ftdi@digilent --boot-mode=jtag --flash-image=flashImg.raw --chip=gap flash wait
    fi
elif [ "$FLAG_BOOT" -eq "1" ]
then
    plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start wait
elif [ "$FLAG_BOOT" -eq "2" ]
then
    plpbridge --cable=ftdi@digilent --boot-mode=jtag_hyper --chip=gap load start wait
fi

