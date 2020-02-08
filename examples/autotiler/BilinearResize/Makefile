# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

APP = Bilinear_Resize

RESIZE_GEN_PATH = $(TILER_GENERATOR_PATH)/Resize
RESIZE_KER_PATH = $(TILER_GENERATOR_PATH)/Resize

APP_SRCS += Bilinear_Resize.c ImgIO.c
APP_SRCS += ResizeKernels.c $(RESIZE_KER_PATH)/ResizeBasicKernels.c

APP_INC += . $(TILER_EMU_INC) $(TILER_INC) $(RESIZE_KER_PATH)

APP_CFLAGS += -O3 -mno-memcpy -fno-tree-loop-distribute-patterns $(USER_FLAGS)
APP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable

MODEL_SRC = ResizeModel.c
MODEL_GEN = ResizeKernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))

RM=rm -f

all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(RESIZE_GEN_PATH) $(MODEL_SRC) $(RESIZE_GEN_PATH)/ResizeGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)
	$(RM) resized_out.ppm

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk

