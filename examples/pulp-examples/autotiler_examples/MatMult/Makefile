# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = MatMult
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
MATMULT_GEN_PATH = $(TILER_GENERATOR_PATH)/MatMult/generator
MATMULT_KER_PATH = $(TILER_GENERATOR_PATH)/MatMult/kernels

PULP_APP_SRCS = TestMat.c MatMultKernelsInit.c MatMultKernels.c $(MATMULT_KER_PATH)/src/MatMultBasicKernels.c
RM=rm -f

PULP_CFLAGS += -O3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns
PULP_LDFLAGS +=  -flto -Wl,--gc-sections
PULP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable
PULP_CFLAGS += -I$(TILER_INC) -I$(MATMULT_KER_PATH)/include

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(MATMULT_GEN_PATH)/include MatMultModel.c $(MATMULT_GEN_PATH)/src/MatMultGenerator.c $(TILER_LIB)

# Run the code generator
MatMultKernels.c MatMultKernelsInit.c: GenTile
	./GenTile

model: MatMultKernels.c MatMultKernelsInit.c

clean::
	$(RM) GenTile MatMultKernels.* MatMultKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
