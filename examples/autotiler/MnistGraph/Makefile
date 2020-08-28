# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

APP = Mnist
RM=rm -f

#To Enable Bridge functions to read/write files from host 
#(this function will be replaced by openocd semihosting in next SDK release)
USE_BRIDGE=0

ifeq ($(USE_BRIDGE),1)
APP_CFLAGS += -DENABLE_BRIDGE
endif
#To test new bridge features:
#comment the line above and use this two instead
READFS_FILES= Mnist_L3_Flash_Const.dat
PLPBRIDGE_FLAGS += -f 
export GAP_USE_OPENOCD=1
io=host


CNN_AT_PATH = $(TILER_CNN_GENERATOR_PATH)

MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_BiasReLULinear_BasicKernels.c
MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_BasicKernels.c
MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_DP_BasicKernels.c
MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_DW_BasicKernels.c
MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_DW_DP_BasicKernels.c
MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_Pooling_BasicKernels.c
MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_MatAlgebra.c
MODEL_LIB_POW2 += $(TILER_CNN_KERNEL_PATH)/CNN_SoftMax.c
MODEL_LIB_POW2 += $(NNTOOL_KERNELS_PATH)/norm_transpose.c
MODEL_LIB_INCLUDE_POW2 = -I$(NNTOOL_KERNELS_PATH) -I$(TILER_CNN_KERNEL_PATH) -I$(NNTOOL_KERNELS_PATH)
MODEL_GEN_POW2 += $(TILER_CNN_GENERATOR_PATH)/CNN_Generator_Util.c
MODEL_GEN_POW2 += $(TILER_CNN_GENERATOR_PATH)/CNN_Generators.c
MODEL_GEN_POW2 += $(NNTOOL_GENERATOR_PATH)/nntool_extra_generators.c
MODEL_GEN_INCLUDE_POW2 = -I$(TILER_CNN_GENERATOR_PATH) -I$(NNTOOL_GENERATOR_PATH)


APP_SRCS += Mnist.c MnistKernels.c \
            $(MODEL_LIB_POW2) \
            ImgIO.c

APP_INC += $(TILER_INC) $(TILER_EMU_INC) $(TILER_CNN_KERNEL_PATH)

APP_CFLAGS += -w -g3 -O3 -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
APP_CFLAGS += -Wno-maybe-uninitialized -Wno-unused-but-set-variable 
LDFLAGS    +=  -flto -Wl,--gc-sections


#Uncomment to use freertos
#PMSIS_OS ?= freertos

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenMnist -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(CNN_AT_PATH) MnistModel.c $(MODEL_GEN_POW2) $(TILER_LIB)

# Run the code generator
MnistKernels.c: GenTile
	./GenMnist

model: MnistKernels.c

clean::
	$(RM) GenMnist MnistKernels.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
