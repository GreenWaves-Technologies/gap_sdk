# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
APP_SRC += main.c
DEMO_SRC += ModelKernels.c ModelKernelsInit.c BasicKernel.c
MODEL_SRC = MatAddModel.c
RM=rm -f

FREERTOS_FLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -D__MBED__
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
INC_PATH += $(TILER_PATH)/StdTypes $(TILER_GENERATOR_PATH)/FFT_Lib

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes/ $(MODEL_SRC) $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
ModelKernels.c ModelKernelsInit.c: GenTile
	./GenTile -m

model: ModelKernels.c ModelKernelsInit.c

clean::
	$(RM) GenTile ModelKernels.* ModelKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/freeRTOS_rules.mk



