export PULP_SDK_HOME=$GAP_SDK_HOME

if [ -n "${GAP_RISCV_GCC_TOOLCHAIN_BASE-}" ]; then
    export GAP_RISCV_GCC_TOOLCHAIN=$GAP_RISCV_GCC_TOOLCHAIN_BASE/1.8
fi

if [ -z "${GAP_RISCV_GCC_TOOLCHAIN-}" ]; then
    export GAP_RISCV_GCC_TOOLCHAIN=/usr/lib/gap_riscv_toolchain
fi

export PATH="$GAP_RISCV_GCC_TOOLCHAIN/bin":"$GAP_SDK_HOME/utils/bin":$PATH

export TARGET_INSTALL_DIR=$GAP_SDK_HOME/install/$TARGET_CHIP
export INSTALL_DIR=$GAP_SDK_HOME/install/workstation
export DEP_DIRS=$INSTALL_DIR
export RULES_DIR=$GAP_SDK_HOME/utils/rules

export NNTOOL_DIR=$GAP_SDK_HOME/tools/nntool
export NNTOOL_PATH=$GAP_SDK_HOME/tools/nntool
export NNTOOL_KERNELS_PATH=$NNTOOL_DIR/autotiler/kernels
export NNTOOL_MATH_PATH=$NNTOOL_DIR/autotiler/math_funcs
export NNTOOL_GENERATOR_PATH=$NNTOOL_DIR/autotiler/generators
export PATH=$PATH:"$GAP_SDK_HOME"
export PATH=$PATH:"$NNTOOL_DIR"

# OpenMP
export OPENMP_DIR="$GAP_SDK_HOME/libs/openmp"

# PulpOS 2
export PULPOS_HOME=$GAP_SDK_HOME/rtos/pulp/pulpos-2
export PULPOS_MODULES="$GAP_SDK_HOME/rtos/pulp/pulpos-2_gap8 $GAP_SDK_HOME/rtos/pulp/pulpos-2_gap9 $GAP_SDK_HOME/rtos/pmsis/pmsis_bsp $OPENMP_DIR $GAP_SDK_HOME/rtos/sfu"
export PULPOS_GAP8_HOME=$GAP_SDK_HOME/rtos/pulp/pulpos-2_gap8
export PULPOS_GAP9_HOME=$GAP_SDK_HOME/rtos/pulp/pulpos-2_gap9
export GAP_PULPOS_ARCHI=$GAP_SDK_HOME/rtos/pulp/gap_archi
export PULPOS_ARCHI=$GAP_SDK_HOME/rtos/pulp/archi_pulp
export PULPOS_HAL=$GAP_SDK_HOME/rtos/pulp/hal_pulp
export PMSIS_API=$GAP_SDK_HOME/rtos/pmsis/pmsis_api

# SFU
export SFU_RUNTIME=$GAP_SDK_HOME/rtos/sfu
export PATH=$GAP_SDK_HOME/tools/sfu_gen/install/bin:$PATH

# For pulp os
export PULP_LIB_DIR=$TARGET_INSTALL_DIR/lib
export PULP_INC_DIR=$TARGET_INSTALL_DIR/include
export RUNTIME_PATH=$GAP_SDK_HOME/pulp-os
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
export GVSOC_ISS_PATH=$GAP_SDK_HOME/gvsoc/gvsoc/models/cpu/iss
export XTENSOR_INCLUDE_DIR=$GAP_SDK_HOME/gvsoc/ext/xtensor/include
export GVSOC_SRC_PATH=$GAP_SDK_HOME/gvsoc/gvsoc
export GVSOC_GAP_SRC_PATH=$GAP_SDK_HOME/gvsoc/gvsoc_gap
export GVSOC_SFU_PATH=$GAP_SDK_HOME/gvsoc/gvsoc_gap_sfu
source $GAP_SDK_HOME/gvsoc/setup_gvsoc.sh
if [ -e "$GAP_SDK_HOME/configs/skip_udma_build" ]; then
    export CONFIG_GVSOC_SKIP_UDMA_BUILD=1
fi
export PYTHONPATH=$GAP_SDK_HOME/gvsoc/gvsoc_gap/models:$PYTHONPATH
export PYTHONPATH=$GAP_SDK_HOME/gvsoc/gvsoc/engine/python:$PYTHONPATH

# gaptest

export PATH="$GAP_SDK_HOME/utils/gaptest":$PATH

# Audio framework
export PYTHONPATH=$GAP_SDK_HOME/tools/audio-framework/frontends/python_graph_generator:$GAP_SDK_HOME/tools/audio-framework/components:$PYTHONPATH


# Autotiler
export AT_HOME=$GAP_SDK_HOME/tools/autotiler_v3
export TILER_PATH=$GAP_SDK_HOME/tools/autotiler_v3
export TILER_LIB=$TILER_PATH/Autotiler/LibTile.a
export TILER_INC=$TILER_PATH/Autotiler
export TILER_EMU_INC=$TILER_PATH/Emulation
export TILER_GENERATOR_PATH=$TILER_PATH/Generators
export TILER_BILINEAR_RESIZE_GENERATOR_PATH=$TILER_GENERATOR_PATH/BilinearResizes
export TILER_BILINEAR_RESIZE_KERNEL_PATH=$TILER_GENERATOR_PATH/BilinearResizes
export TILER_INTEGRAL_GENERATOR_PATH=$TILER_GENERATOR_PATH/IntegralImage
export TILER_INTEGRAL_KERNEL_PATH=$TILER_GENERATOR_PATH/IntegralImage
export TILER_FFT2D_GENERATOR_PATH=$TILER_GENERATOR_PATH/FFT2DModel
export TILER_FFT2D_KERNEL_PATH=$TILER_GENERATOR_PATH/FFT2DModel
export TILER_FFT2D_TWIDDLE_PATH=$TILER_GENERATOR_PATH/FFT2DModel
export TILER_CNN_KERNEL_PATH=$TILER_PATH/CNN_Libraries
export TILER_CNN_GENERATOR_PATH=$TILER_PATH/CNN_Generators
export TILER_CNN_KERNEL_PATH_SQ8=$TILER_PATH/CNN_Libraries_SQ8
export TILER_CNN_KERNEL_PATH_NE16=$TILER_PATH/CNN_Libraries_NE16
export TILER_CNN_KERNEL_PATH_FP16=$TILER_PATH/CNN_Libraries_fp16
export TILER_CNN_GENERATOR_PATH_SQ8=$TILER_PATH/CNN_Generators_SQ8
export TILER_CNN_GENERATOR_PATH_NE16=$TILER_PATH/CNN_Generators_NE16
export TILER_CNN_GENERATOR_PATH_FP16=$TILER_PATH/CNN_Generators_fp16

export TILER_DSP_GENERATOR_PATH=$TILER_PATH/DSP_Generators
export TILER_DSP_KERNEL_PATH=$TILER_PATH/DSP_Libraries
export TILER_FFT_LUT_PATH=$TILER_PATH/DSP_Libraries/LUT_Tables
export TILER_MFCC_GEN_LUT_SCRIPT=$TILER_PATH/DSP_Libraries/LUT_Tables/gen_scripts/GenMFCCLUT.py
export TILER_FFT_GEN_LUT_SCRIPT=$TILER_PATH/DSP_Libraries/LUT_Tables/gen_scripts/GenFFTLUT.py
export TILER_WIN_GEN_LUT_SCRIPT=$TILER_PATH/DSP_Libraries/LUT_Tables/gen_scripts/GenWinLUT.py

# OpenOCD
export GAP_OPENOCD_TOOLS=$GAP_SDK_HOME/utils/openocd_tools
export GAP_USE_OPENOCD=1

# Zephyr
export CROSS_COMPILE="$GAP_RISCV_GCC_TOOLCHAIN/bin/riscv32-unknown-elf-"
export ZEPHYR_GCC_VARIANT=cross-compile

# Gapy
export PATH=$GAP_SDK_HOME/utils/gapy:$PATH
export PYTHONPATH=$GAP_SDK_HOME/utils/gap_configs/python:$PYTHONPATH
export PYTHONPATH=$GAP_SDK_HOME/utils/gapy:$PYTHONPATH
export PULP_CONFIGS_PATH=$GAP_SDK_HOME/utils/gap_configs/configs:$PULP_CONFIGS_PATH

# GAP LIB
export GAP_LIB_PATH=$GAP_SDK_HOME/libs/gap_lib

if [ "$TARGET_CHIP_FAMILY" = "GAP8" ]
then
    PROJECT_PATH=$(dirname $GAP_SDK_HOME)
    export VSIM_PATH=$PROJECT_PATH/fe/sim
    export GAP_OPENOCD_PATH=gap8-openocd
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

if [ -f "$GAP_SDK_HOME/configs/wsl.sh" ]; then
    source $GAP_SDK_HOME/configs/wsl.sh
fi
