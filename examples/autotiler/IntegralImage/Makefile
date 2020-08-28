# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
pulpChip = GAP
PULP_APP = IntegralImage

GEN_KER_PATH = $(TILER_INTEGRAL_GENERATOR_PATH)

MODEL_SRC = IntegralImgModel.c
MODEL_GEN = IntegralImgKernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
RM=rm -f

APP_SRCS   += main.c $(MODEL_GEN_C) $(GEN_KER_PATH)/IntegralImgBasicKernels.c $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_INC     = $(GAP_LIB_PATH)/include

APP_CFLAGS += -O3 -mno-memcpy -fno-tree-loop-distribute-patterns $(JENKINS_FLAGS)
APP_CFLAGS += -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(GEN_KER_PATH)

all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(GEN_KER_PATH) $(MODEL_SRC) $(GEN_KER_PATH)/IntegralImgGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
