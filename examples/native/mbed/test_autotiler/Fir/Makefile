# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
FIR_GEN_PATH = $(TILER_GENERATOR_PATH)/Fir/generator
FIR_KER_PATH = $(TILER_GENERATOR_PATH)/Fir/kernels

MODEL_SRC = FirModel.c
MODEL_GEN = FirKernels FirKernelsInit
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
RM=rm -f

TEST_C = main.c $(MODEL_GEN_C) $(FIR_KER_PATH)/src/FirBasicKernels.c
INC_PATH += -I$(TILER_INC) -I$(FIR_KER_PATH)/include

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(FIR_GEN_PATH)/include $(MODEL_SRC) $(FIR_GEN_PATH)/src/FirGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile -m

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
