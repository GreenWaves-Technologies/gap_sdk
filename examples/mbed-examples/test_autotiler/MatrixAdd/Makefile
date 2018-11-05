# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
MATADD_GEN_PATH = $(TILER_GENERATOR_PATH)/MatAdd/generator
MATADD_KER_PATH = $(TILER_GENERATOR_PATH)/MatAdd/kernels

TEST_C = main.c MatAddKernels.c MatAddKernelsInit.c $(MATADD_KER_PATH)/src/MatAddBasicKernels.c
MODEL_SRC = MatAddModel.c

RM=rm -f

MBED_FLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
INC_PATH += -I$(TILER_INC) -I$(MATADD_KER_PATH)/include

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(MATADD_GEN_PATH)/include $(MODEL_SRC) $(MATADD_GEN_PATH)/src/MatAddGenerator.c $(TILER_LIB)

# Run the code generator
MatAddKernels.c MatAddKernelsInit.c: GenTile
	./GenTile -m

model: MatAddKernels.c MatAddKernelsInit.c

clean::
	$(RM) GenTile MatAddKernels.* MatAddKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk


