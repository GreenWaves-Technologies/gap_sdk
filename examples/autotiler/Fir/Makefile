# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = TestFIR
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler_v3
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
FIR_GEN_PATH = $(TILER_GENERATOR_PATH)/Fir
#FIR_KER_PATH = $(TILER_GENERATOR_PATH)/Fir

MODEL_SRC = FirModel.c
MODEL_GEN = FirKernels 
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
RM=rm -f

PULP_APP_SRCS = main.c $(MODEL_GEN_C) $(FIR_GEN_PATH)/FirBasicKernels.c

PULP_CFLAGS += -O3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns
#PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
PULP_CFLAGS += -I$(TILER_INC) -I$(FIR_GEN_PATH)
PULP_LDFLAGS +=  -flto -Wl,--gc-sections

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(FIR_GEN_PATH) $(MODEL_SRC) $(FIR_GEN_PATH)/FirGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
