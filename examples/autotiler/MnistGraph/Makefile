# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

APP = Mnist
RM=rm -f

#To Enable Bridge functions to read/write files from host 
#(this function will be replaced by openocd semihosting in next SDK release)
USE_BRIDGE=1

ifeq ($(USE_BRIDGE),1)
APP_CFLAGS += -DENABLE_BRIDGE
endif
#To test new bridge features:
#comment the line above and use this two instead
READFS_FILES= Mnist_L3_Flash_Const.dat
PLPBRIDGE_FLAGS += -f 
io=host

MODEL_POW2 = 1
include $(RULES_DIR)/at_common_decl.mk

APP_SRCS += Mnist.c MnistKernels.c \
            $(CNN_LIB) \
            ImgIO.c

APP_INC += $(TILER_INC) $(TILER_EMU_INC)

APP_CFLAGS += -w -O3 -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
APP_CFLAGS += -Wno-maybe-uninitialized -Wno-unused-but-set-variable $(CNN_LIB_INCLUDE)
LDFLAGS    += -flto -Wl,--gc-sections

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenMnist -fcommon -I$(TILER_INC) -I$(TILER_EMU_INC) $(CNN_GEN_INCLUDE) MnistModel.c $(CNN_GEN) $(TILER_LIB)

# Run the code generator
MnistKernels.c: GenTile
	./GenMnist

model: MnistKernels.c

clean::
	$(RM) GenMnist MnistKernels.*

.PHONY: model clean

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
