# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

RM=rm -f
CNN_AT_PATH = $(TILER_CNN_GENERATOR_PATH)

APP = Cifar10

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


APP_SRCS += Cifar10.c Cifar10Kernels.c \
			$(MODEL_LIB_POW2)
            
APP_INC += $(TILER_INC) $(TILER_EMU_INC) $(TILER_CNN_KERNEL_PATH)

COEF_FLAG = -DCOEF_L2

APP_CFLAGS += -O2 -mno-memcpy -fno-tree-loop-distribute-patterns  -fdata-sections -ffunction-sections
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
APP_CFLAGS += -w -Wno-maybe-uninitialized -Wno-unused-but-set-variable $(COEF_FLAG)
#APP_CFLAGS += -I$(TILER_INC) -I$(CNN_AT_PATH)

LDFLAGS +=  -flto -Wl,--gc-sections

PLPBRIDGE_FLAGS += -jtag

# Files to copy in flash
FILE_BIAS0   = coef/Cifar10_Bias0.dat
FILE_BIAS1   = coef/Cifar10_Bias1.dat
FILE_BIAS2   = coef/Cifar10_Bias2.dat
FILE_FILTER0 = coef/Cifar10_Filter0.dat
FILE_FILTER1 = coef/Cifar10_Filter1.dat
FILE_FILTER2 = coef/Cifar10_Filter2.dat

FLASH_FILES = $(FILE_BIAS0) $(FILE_BIAS1) $(FILE_BIAS2) $(FILE_FILTER0) $(FILE_FILTER1) $(FILE_FILTER2)

#This says to put the files into flash image
READFS_FILES=$(realpath $(FLASH_FILES)) 
# This says to bridge to copy the image into flash
PLPBRIDGE_FLAGS = -f


#PMSIS_OS ?= freertos

ifeq ('$(BOARD_NAME)', 'gapuino')
APP_CFLAGS += -DGAPUINO
else ifeq ('$(BOARD_NAME)', 'gapoc_a')
PLPBRIDGE_FLAGS += -ftdi
APP_CFLAGS += -DGAPOC_A
endif

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenCifar10 $(COEF_FLAG) -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(CNN_AT_PATH) Cifar10Model.c $(MODEL_GEN_POW2) $(TILER_LIB)

# Run the code generator
Cifar10Kernels.c: GenTile
	./GenCifar10

model: Cifar10Kernels.c

clean::
	$(RM) GenCifar10 Cifar10Kernels.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk

