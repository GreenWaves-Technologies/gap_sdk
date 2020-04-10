# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = HoG
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler_v2
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators

GEN_PATH = $(TILER_GENERATOR_PATH)/HoG/generator/src
KER_PATH = $(TILER_GENERATOR_PATH)/HoG/kernels/src

MODEL_SRC = $(GEN_PATH)/HoGGenerator.c HoGModel.c
MODEL_GEN = HoGKernels HoGKernelsInit
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .c, $(MODEL_GEN)) $(addsuffix .h, $(MODEL_GEN))
pulpFc=1
RM=rm -f

APP_SRCS = HoG.c ImgIO.c $(MODEL_GEN_C) $(KER_PATH)/HoGBasicKernels.c

APP_CFLAGS += -O2 -g3 #-DNO_BRIDGE
APP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef 
APP_CFLAGS += -I$(TILER_INC) -I$(KER_PATH)/../include -I$(TILER_GENERATOR_PATH)/HoG
APP_LDFLAGS +=  -flto -Wl,--gc-sections

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
APP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable

#moved to new openocd bridge
export GAP_USE_OPENOCD=1
io=host

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(KER_PATH)/../include -I$(GEN_PATH)/../include  $(MODEL_SRC) $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk


