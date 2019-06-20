export GAP_SDK_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PULP_SDK_HOME=$GAP_SDK_HOME
export GAP_RISCV_GCC_TOOLCHAIN=/usr/lib/gap_riscv_toolchain
export TARGET_CHIP="GAP8"
export TARGET_NAME="gap"
export PULP_CURRENT_CONFIG=gap@config_file=chips/gap/gap.json

export TARGET_INSTALL_DIR=$GAP_SDK_HOME/install/$TARGET_CHIP
export BUILD_DIR=$GAP_SDK_HOME/build
export INSTALL_DIR=$GAP_SDK_HOME/install/workstation
export DEP_DIRS=$INSTALL_DIR

# For pulp os
export PULP_LIB_DIR=$TARGET_INSTALL_DIR/lib
export PULP_INC_DIR=$TARGET_INSTALL_DIR/include
export RUNTIME_PATH=$GAP_SDK_HOME/pulp-os
# For mbed os
export MBED_PATH=$GAP_SDK_HOME
# For TF2GAP8
export GAP_TF2GAP_PATH=$GAP_SDK_HOME/tf2gap8
# For FreeRTOS
export FREERTOS_PATH=$GAP_SDK_HOME/freeRTOS

export PATH="$INSTALL_DIR/bin":$PATH
export LD_LIBRARY_PATH="$INSTALL_DIR/lib":$LD_LIBRARY_PATH
export PYTHONPATH=$INSTALL_DIR/python:$PYTHONPATH
export PULP_CONFIGS_PATH=$INSTALL_DIR/configs

# For GVSOC
export PULP_RISCV_GCC_TOOLCHAIN=$GAP_RISCV_GCC_TOOLCHAIN
export PULP_SDK_INSTALL=$INSTALL_DIR
source $GAP_SDK_HOME/gvsoc/setup_gvsoc.sh

