# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_GENERATOR_PATH=$(TILER_PATH)/autotiler_generator
TEST_C = Mnist.c MnistKernels.c MnistKernelsInit.c $(TILER_GENERATOR_PATH)/CnnStdModel/CNN_BasicKernels.c
ifdef RT_HAS_HWCE
	PULP_APP_SRCS += $(TILER_GENERATOR_PATH)/CnnStdModel/CNN_HwCE.c
endif
RM=rm -f

MBED_FLAGS += -O3
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
MBED_FLAGS += -Wall -Wno-maybe-uninitialized -Wno-unused-but-set-variable -Wno-unused-variable
INC_PATH += -I$(TILER_PATH)/StdTypes -I$(TILER_GENERATOR_PATH)/CnnStdModel

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenMnist -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes/ -I$(TILER_GENERATOR_PATH)/CnnStdModel MnistGenerator.c $(TILER_GENERATOR_PATH)/CnnStdModel/CNN_Generator.c $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
MnistKernels.c MnistKernelsInit.c: GenTile
	./GenMnist -m

model: MnistKernels.c MnistKernelsInit.c

clean::
	$(RM) GenMnist MnistKernels.* MnistKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

