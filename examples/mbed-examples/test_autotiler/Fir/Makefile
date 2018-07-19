# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_GENERATOR_PATH=$(TILER_PATH)/autotiler_generator
MODEL_SRC = $(TILER_GENERATOR_PATH)/Fir/FirGenerator.c FirModelExample.c
MODEL_GEN = FirKernels FirKernelsInit
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .c, $(MODEL_GEN))
RM=rm -f

TEST_C = main.c $(MODEL_GEN_C) FirBasicKernels.c

INC_PATH += -I$(TILER_PATH)/StdTypes/

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_GENERATOR_PATH)/Fir -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes/ $(MODEL_SRC) $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile -m

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
