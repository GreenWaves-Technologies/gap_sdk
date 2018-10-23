# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = MatrixAdd
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
MATADD_GEN_PATH = $(TILER_GENERATOR_PATH)/MatAdd/generator
MATADD_KER_PATH = $(TILER_GENERATOR_PATH)/MatAdd/kernels

PULP_APP_SRCS = main.c MatAddKernels.c MatAddKernelsInit.c $(MATADD_KER_PATH)/src/MatAddBasicKernels.c
MODEL_SRC = MatAddModel.c
pulpFc=1
RM=rm -f

PULP_CFLAGS += -O3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable
PULP_CFLAGS += -I$(TILER_INC) -I$(MATADD_KER_PATH)/include

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(MATADD_GEN_PATH)/include $(MODEL_SRC) $(MATADD_GEN_PATH)/src/MatAddGenerator.c $(TILER_LIB)

# Run the code generator
MatAddKernels.c MatAddKernelsInit.c: GenTile
	./GenTile

model: MatAddKernels.c MatAddKernelsInit.c

clean::
	$(RM) GenTile MatAddKernels.* MatAddKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
