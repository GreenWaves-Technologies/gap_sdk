# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

APP = MatMult

MATMULT_GEN_PATH = $(TILER_GENERATOR_PATH)/MatMult
MATMULT_KER_PATH = $(TILER_GENERATOR_PATH)/MatMult

APP_SRCS += MatMult.c
APP_SRCS += MatMultKernels.c $(MATMULT_KER_PATH)/MatMultBasicKernels.c

APP_INC += . $(TILER_INC) $(TILER_EMU_INC) $(MATMULT_KER_PATH)

APP_CFLAGS += -O3 -mno-memcpy -fno-tree-loop-distribute-patterns
APP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable -Wno-unused-variable

MODEL_SRC = MatMultModel.c
MODEL_GEN = MatMultKernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))

RM=rm -f

all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(MATMULT_GEN_PATH) $(MODEL_SRC) $(MATMULT_GEN_PATH)/MatMultGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
