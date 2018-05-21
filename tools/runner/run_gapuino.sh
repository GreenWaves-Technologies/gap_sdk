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

case ${1} in
    -s) script_filename="$(echo $2 | cut -d'@' -f2 )"
        cp $here$script_filename .
        plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start script --script=$2
        ;;
    -gdb)
        plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap --config=$INSTALL_DIR/config/config.json load ioloop start gdb wait
        ;;
    -f)
        buildFlashImage="flashImageBuilder --verbose --flash-boot-binary=test --raw=flashImg.raw"
        while [ $# -gt 1  ]
        do
            buildFlashImage="$buildFlashImage --comp=$here/$2"
            shift
        done
        eval $buildFlashImage
#        plpbridge --verbose=4 --cable=ftdi@digilent --boot-mode=jtag_hyper --chip=gap --binary=flashImg.raw --config=$GAP_SDK_HOME/tools/bin/config.json load ioloop start
        ;;
    *)
        plpbridge --cable=ftdi@digilent --boot-mode=jtag --binary=test --chip=gap load ioloop reqloop start wait
        ;;
esac

