#!/bin/sh

echo ""
echo ""
echo "          ,ad8888ba,        db        88888888ba   ad88888ba   "
echo "         d8\"'    \`\"8b      d88b       88      \"8b d8\"     \"8b  "
echo "        d8'               d8'\`8b      88      ,8P Y8a     a8P  "
echo "        88               d8'  \`8b     88aaaaaa8P'  \"Y8aaa8P\"   "
echo "        88      88888   d8YaaaaY8b    88\"\"\"\"\"\"'    ,d8\"\"\"8b,   "
echo "        Y8,        88  d8\"\"\"\"\"\"\"\"8b   88          d8\"     \"8b  "
echo "         Y8a.    .a88 d8'        \`8b  88          Y8a     a8P  "
echo "          \`\"Y88888P\" d8'          \`8b 88           \"Y88888P\"   "

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
    plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start wait
else
    while [ "$#" -gt 0 ]
    do
        case ${1} in
            -s) script_filename="$(echo $2 | cut -d'@' -f2 )"
                cp $here/$script_filename .
                plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start script --script=$2
                ;;
            -gdb)
                plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap --config=$INSTALL_DIR/config/config_jtag.json load ioloop start gdb wait
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
        plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag --chip=gap --fimage=flashImg.raw --config=$INSTALL_DIR/config/config_jtag.json load ioloop start flash wait
        plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start wait
    # Flash the flash image and boot from hyper
    elif [ "$FLAG_BOOT" -eq "2" ]
    then
        plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag --chip=gap --fimage=flashImg.raw --config=$INSTALL_DIR/config/config_jtag.json load ioloop start flash wait
        plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag_hyper --chip=gap --config=$INSTALL_DIR/config/config_hyper.json load start wait
    # Flash the flash image only
    else
        plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag --chip=gap --fimage=flashImg.raw --config=$INSTALL_DIR/config/config_jtag.json load ioloop start flash wait
    fi
elif [ "$FLAG_BOOT" -eq "1" ]
then
    plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start wait
elif [ "$FLAG_BOOT" -eq "2" ]
then
    plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag_hyper --chip=gap --config=$INSTALL_DIR/config/config_hyper.json load start wait
fi

