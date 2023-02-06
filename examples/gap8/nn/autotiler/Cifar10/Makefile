# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.


# All the coef in L2 statically, 
# otherwise, in the external flash
#COEF_FLAG = -DCOEF_L2

RM=rm -f
CNN_AT_PATH = $(TILER_CNN_GENERATOR_PATH)

APP = Cifar10

MODEL_POW2 = 1
include $(RULES_DIR)/at_common_decl.mk

APP_SRCS += Cifar10.c Cifar10Kernels.c $(CNN_LIB)
            
APP_INC += $(TILER_INC) $(TILER_EMU_INC)

APP_CFLAGS += -O3 -mno-memcpy -fno-tree-loop-distribute-patterns  -fdata-sections -ffunction-sections
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
APP_CFLAGS += -w -Wno-maybe-uninitialized -Wno-unused-but-set-variable $(COEF_FLAG) $(CNN_LIB_INCLUDE)
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

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenCifar10: Cifar10Model.c
	gcc -o GenCifar10 -fcommon $(COEF_FLAG) -I$(TILER_INC) -I$(TILER_EMU_INC) $(CNN_GEN_INCLUDE) Cifar10Model.c $(CNN_GEN) $(TILER_LIB)

# Run the code generator
Cifar10Kernels.c: GenCifar10
	./GenCifar10

model: Cifar10Kernels.c

clean::
	$(RM) GenCifar10 Cifar10Kernels.*

.PHONY: model clean

include $(RULES_DIR)/pmsis_rules.mk

