# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
MATMULT_GEN_PATH = $(TILER_GENERATOR_PATH)/MatMult/generator
MATMULT_KER_PATH = $(TILER_GENERATOR_PATH)/MatMult/kernels

TEST_C = TestMat.c $(MATMULT_KER_PATH)/src/MatMultBasicKernels.c MatMultKernelsInit.c MatMultKernels.c
RM=rm -f

INC_PATH += -I$(TILER_INC) -I$(MATMULT_KER_PATH)/include

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(MATMULT_GEN_PATH)/include MatMultModel.c $(MATMULT_GEN_PATH)/src/MatMultGenerator.c $(TILER_LIB)

# Run the code generator
MatMultKernels.c MatMultKernelsInit.c: GenTile
	./GenTile -m

model: MatMultKernels.c MatMultKernelsInit.c

clean::
	$(RM) GenTile MatMultKernels.* MatMultKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
