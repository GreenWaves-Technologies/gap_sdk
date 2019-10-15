#!/bin/bash
mkdir -p vectors slm_files
objcopy --srec-len 1 --output-target=srec test test.s19
parse_s19.pl test.s19 > ./vectors/stim.txt
cd slm_files && s19toslm-new.py ../test.s19 GAP None
cd ..

export VSIM_DESIGN_MODEL=sverilog
export SPI_FLASH_LOAD_MEM=YES

export VSIM_RUNNER_FLAGS="+preload_file=efuse_preload.data -gLOAD_L2=STANDALONE -warning 3197,3748"
# Boot from SPI by default
cp $GAP_SDK_HOME/tools/efuse/efuse_spi_preload.data ./efuse_preload.data

USE_GUI=0
FLASH_TYPE="spi"

FILE_IMAGE=""

while [ "$#" -gt 0 ]
do
    # For TRACE
    if [ "$1" == "YES" ]
    then
        export RECORD_WLF=YES
        shift
        export DO_FILES=$1
    fi

    # For HYPER
    if [ "$1" == "TB_PADMUX_ALT3_HYPERBUS" ]
    then
        export VSIM_PADMUX_CFG=$1
    fi

    # For SPI SLAVE
    if [ "$1" == "TB_PADMUX_SPIM_SPIS_LOOPBACK" ]
    then
        export VSIM_PADMUX_CFG=$1
    fi

    # For BOOT
    if [ "$1" == "TB_BOOT_FROM_HYPER_FLASH" ]
    then
        unset SPI_FLASH_LOAD_MEM
        export VSIM_RUNNER_FLAGS="-gHYPER_FLASH_LOAD_MEM=1 +VSIM_BOOTTYPE_CFG=TB_BOOT_FROM_HYPER_FLASH $VSIM_RUNNER_FLAGS"
        FLASH_TYPE="hyper"
        export VSIM_PADMUX_CFG=TB_PADMUX_ALT3_HYPERBUS
        cp $GAP_SDK_HOME/tools/efuse/efuse_hyper_preload.data ./efuse_preload.data
    fi

    if [ "$1" == "TB_BOOT_FROM_SPI_FLASH" ]
    then
        export VSIM_BOOTTYPE_CFG=$1
    fi

    if [ "$1" == "GUI" ]
    then
        USE_GUI=1
    fi

    if [ "$1" == "-f" ]
    then
        shift
        FILE_IMAGE="$1"
    fi

    shift
done

if [ "$FILE_IMAGE" != "" ]
then
    plp_mkflash  --flash-boot-binary=test  --stimuli=slm_files/flash_stim.slm --flash-type=$FLASH_TYPE --qpi --verbose --archi=GAP-riscv --comp=../../../$FILE_IMAGE
else
    plp_mkflash  --flash-boot-binary=test  --stimuli=slm_files/flash_stim.slm --flash-type=$FLASH_TYPE --qpi --verbose --archi=GAP-riscv
fi

if [ "$USE_GUI" == "1" ]
then
    export VOPT_ACC_ENA=YES
    eval "vsim -64 -do 'source $VSIM_PATH/tcl_files/config/run_and_exit.tcl' -do 'source $VSIM_PATH/tcl_files/run.tcl'"
else
    eval "vsim -64 -c -do 'source $VSIM_PATH/tcl_files/config/run_and_exit.tcl' -do 'source $VSIM_PATH/tcl_files/run.tcl; run_and_exit;'"
fi
