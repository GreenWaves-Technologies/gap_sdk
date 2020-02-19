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
PLPBRIDGE_FLAGS += -fileIO
#To test new bridge features:
#comment the line above and use this two instead
#export GAP_USE_OPENOCD=1
#io=host
endif

CNN_AT_PATH = $(TILER_GENERATOR_PATH)/CNN

APP_SRCS += Mnist.c MnistKernels.c \
            $(CNN_AT_PATH)/CNN_BiasReLULinear_BasicKernels.c \
            $(CNN_AT_PATH)/CNN_Conv_BasicKernels.c \
            $(CNN_AT_PATH)/CNN_Pooling_BasicKernels.c \
            ImgIO.c

APP_INC += $(TILER_INC) $(CNN_AT_PATH)

APP_CFLAGS += -O2 -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
APP_CFLAGS += -Wno-maybe-uninitialized -Wno-unused-but-set-variable 
LDFLAGS    +=  -flto -Wl,--gc-sections


#Uncomment to use freertos
#PMSIS_OS ?= freerto
USE_PMSIS_BSP = 1

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenMnist -I$(TILER_INC) -I$(CNN_AT_PATH) MnistModel.c $(CNN_AT_PATH)/CNN_Generators.c $(TILER_LIB)

# Run the code generator
MnistKernels.c: GenTile
	./GenMnist

model: MnistKernels.c

clean::
	$(RM) GenMnist MnistKernels.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
