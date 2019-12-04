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
export PATH="$NNTOOL_DIR":$PATH

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
source $GAP_SDK_HOME/gvsoc/setup_gvsoc.sh

# Autotiler
export TILER_PATH=$GAP_SDK_HOME/tools/autotiler_v3
export TILER_LIB=$TILER_PATH/lib/libtile.a
export TILER_INC=$TILER_PATH/include
export TILER_EMU_INC=$TILER_PATH/include
export TILER_GENERATOR_PATH=$TILER_PATH/generators

# OpenOCD
export GAP_OPENOCD_TOOLS=$INSTALL_DIR/share/openocd

if [ "$TARGET_CHIP" = "GAP8" ]
then
    PROJECT_PATH=$(dirname $GAP_SDK_HOME)
    export VSIM_PATH=$PROJECT_PATH/fe/sim
else
    # For NEW SDK rtl test
    PROJECT_PATH=$(dirname $(dirname $GAP_SDK_HOME))
    export VSIM_PATH=$PROJECT_PATH/sim/vsim
    export XCSIM_PATH=$PROJECT_PATH/sim/xcsim
    export XCSIM_PLATFORM=$XCSIM_PATH
fi
