#! /bin/bash

export PULP_SDK_HOME=$GAP_SDK_HOME

if [ -z "$GAP_RISCV_GCC_TOOLCHAIN" ]; then
    export GAP_RISCV_GCC_TOOLCHAIN=/usr/lib/gap_riscv_toolchain
fi

export PATH="$GAP_RISCV_GCC_TOOLCHAIN/bin":"$GAP_SDK_HOME/tools/bin":$PATH

export TARGET_INSTALL_DIR=$GAP_SDK_HOME/install/$TARGET_CHIP
export INSTALL_DIR=$GAP_SDK_HOME/install/workstation
export DEP_DIRS=$INSTALL_DIR
export RULES_DIR=$GAP_SDK_HOME/tools/rules

export NNTOOL_DIR=$GAP_SDK_HOME/tools/nntool
export NNTOOL_PATH=$GAP_SDK_HOME/tools/nntool
export NNTOOL_KERNELS_PATH=$NNTOOL_DIR/autotiler/kernels
export NNTOOL_GENERATOR_PATH=$NNTOOL_DIR/autotiler/generators
export PATH="$NNTOOL_DIR":$PATH

# PulpOS 2
export PULPOS_HOME=$GAP_SDK_HOME/rtos/pulp/pulpos-2
export PULPOS_MODULES="$GAP_SDK_HOME/rtos/pulp/pulpos-2_gap8 $GAP_SDK_HOME/rtos/pulp/pulpos-2_gap9 $GAP_SDK_HOME/rtos/pmsis/pmsis_bsp"
export PULPOS_GAP8_HOME=$GAP_SDK_HOME/rtos/pulp/pulpos-2_gap8
export PULPOS_GAP9_HOME=$GAP_SDK_HOME/rtos/pulp/pulpos-2_gap9
export GAP_PULPOS_ARCHI=$GAP_SDK_HOME/rtos/pulp/gap_archi
export PULPOS_ARCHI=$GAP_SDK_HOME/rtos/pulp/archi_pulp
export PULPOS_HAL=$GAP_SDK_HOME/rtos/pulp/hal_pulp
export PMSIS_API=$GAP_SDK_HOME/rtos/pmsis/pmsis_api


# For pulp os
export PULP_LIB_DIR=$TARGET_INSTALL_DIR/lib
export PULP_INC_DIR=$TARGET_INSTALL_DIR/include
export RUNTIME_PATH=$GAP_SDK_HOME/pulp-os
# For mbed os
export MBED_PATH=$GAP_SDK_HOME/rtos
# For FreeRTOS
export FREERTOS_PATH=$GAP_SDK_HOME/rtos/freeRTOS

export PATH="$INSTALL_DIR/bin":$PATH
export LD_LIBRARY_PATH="$INSTALL_DIR/lib":$LD_LIBRARY_PATH
export PYTHONPATH=$INSTALL_DIR/python:$PYTHONPATH
export PULP_CONFIGS_PATH=$INSTALL_DIR/configs

# For GVSOC
export PULP_RISCV_GCC_TOOLCHAIN=$GAP_RISCV_GCC_TOOLCHAIN
export PULP_SDK_INSTALL=$INSTALL_DIR
export GVSOC_PATH=$INSTALL_DIR/python
source $GAP_SDK_HOME/gvsoc/setup_gvsoc.sh

# Autotiler
export TILER_PATH=$GAP_SDK_HOME/tools/autotiler_v3
export TILER_LIB=$TILER_PATH/lib/libtile.a
export TILER_INC=$TILER_PATH/include
export TILER_EMU_INC=$TILER_PATH/include
export TILER_GENERATOR_PATH=$TILER_PATH/generators
export TILER_CNN_KERNEL_PATH=$TILER_PATH/generators/CNN
export TILER_CNN_GENERATOR_PATH=$TILER_PATH/generators/CNN
export TILER_CNN_KERNEL_PATH_SQ8=$TILER_PATH/generators/CNN
export TILER_CNN_GENERATOR_PATH_SQ8=$TILER_PATH/generators/CNN


# OpenOCD
export GAP_OPENOCD_TOOLS=$GAP_SDK_HOME/tools/gap8-openocd-tools
export GAP_USE_OPENOCD=1

# Zephyr
export CROSS_COMPILE="$GAP_RISCV_GCC_TOOLCHAIN/bin/riscv32-unknown-elf-"
export ZEPHYR_GCC_VARIANT=cross-compile

# Gapy
export PATH=$GAP_SDK_HOME/tools/gapy:$PATH
export PYTHONPATH=$GAP_SDK_HOME/tools/gap-configs/python:$PYTHONPATH
export PYTHONPATH=$GAP_SDK_HOME/tools/gapy:$PYTHONPATH
export PULP_CONFIGS_PATH=$GAP_SDK_HOME/tools/gap-configs/configs:$PULP_CONFIGS_PATH

# GAP LIB
export GAP_LIB_PATH=$GAP_SDK_HOME/libs/gap_lib

if [ "$TARGET_CHIP" = "GAP8" ]
then
    PROJECT_PATH=$(dirname $GAP_SDK_HOME)
    export VSIM_PATH=$PROJECT_PATH/fe/sim
else
    # For NEW SDK rtl test
    PROJECT_PATH=$(dirname $(dirname $GAP_SDK_HOME))
    if [ -z "$VSIM_PATH" ]; then
        export VSIM_PATH=$PROJECT_PATH/sim/vsim
    fi
    if [ -z "$XCSIM_PATH" ]; then
        export XCSIM_PATH=$PROJECT_PATH/sim/xcsim
        export XCSIM_PLATFORM=$XCSIM_PATH
    fi
fi
