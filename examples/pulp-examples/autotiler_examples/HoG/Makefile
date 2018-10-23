# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = HoG
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
GEN_PATH = $(TILER_GENERATOR_PATH)/HoG/generator
KER_PATH = $(TILER_GENERATOR_PATH)/HoG/kernels

MODEL_SRC = $(GEN_PATH)/src/HoGGenerator.c HoGModel.c
MODEL_GEN = HoGKernels HoGKernelsInit
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .c, $(MODEL_GEN)) $(addsuffix .h, $(MODEL_GEN))
pulpFc=1
RM=rm -f

PULP_APP_SRCS = HoG.c ImgIO.c $(MODEL_GEN_C) $(KER_PATH)/src/HoGBasicKernels.c

PULP_CFLAGS += -O2 -g3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
PULP_CFLAGS += -I$(TILER_INC) -I$(KER_PATH)/include -I$(TILER_GENERATOR_PATH)/HoG
PULP_LDFLAGS +=  -flto -Wl,--gc-sections

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(KER_PATH)/include -I$(GEN_PATH)/include  $(MODEL_SRC) $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk


