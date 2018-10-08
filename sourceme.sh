export GAP_SDK_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PULP_SDK_HOME=$GAP_SDK_HOME
export GAP_RISCV_GCC_TOOLCHAIN=/usr/lib/gap_riscv_toolchain

export TARGET_INSTALL_DIR=$GAP_SDK_HOME/install/gap
export BUILD_DIR=$GAP_SDK_HOME/build
export INSTALL_DIR=$GAP_SDK_HOME/install/workstation
export DEP_DIRS=$INSTALL_DIR
export LIB_DIR=$TARGET_INSTALL_DIR/libs

# For pulp os
export RUNTIME_PATH=$GAP_SDK_HOME/pulp-os
# For mbed os
export MBED_PATH=$GAP_SDK_HOME
# For TF2GAP8
export GAP_TF2GAP_PATH=$GAP_SDK_HOME/tf2gap8
# For FreeRTOS
export FREERTOS_PATH=$GAP_SDK_HOME/freeRTOS

export PATH="$INSTALL_DIR/bin":$PATH
export LD_LIBRARY_PATH="$INSTALL_DIR/lib":$LD_LIBRARY_PATH
export PYTHONPATH=$INSTALL_DIR/python

# For New SDK rtl test
# export VSIM_PATH=$MBED_PATH/../rtl/gap/fe/sim

# For SDK rtl test
export VSIM_PATH=$MBED_PATH/../fe/sim
