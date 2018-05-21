# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = IntegralImage
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
MODEL_SRC = IIGenerator.c IIModelExample.c

MODEL_GEN = IIKernels IIKernelsInit
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
pulpFc=1
RM=rm -f

PULP_APP_SRCS = main.c ImgIO.c $(MODEL_GEN_C) IIBasicKernels.c 

PULP_CFLAGS += -O2 -g3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
PULP_CFLAGS += -I$(TILER_PATH)/StdTypes/
PULP_LDFLAGS +=  -flto -Wl,--gc-sections

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes/ $(MODEL_SRC) $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk


