# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

RM=rm -f

RESIZE_GEN_PATH = $(TILER_GENERATOR_PATH)/Resize
RESIZE_KER_PATH = $(TILER_GENERATOR_PATH)/Resize

APP = test_resize
APP_SRCS += test_resize.c ImgIO.c
APP_INC += . $(TILER_INC)
APP_CFLAGS +=

PLPBRIDGE_FLAGS += -fileIO 2

PMSIS_OS ?= freertos
BOARD_NAME ?= gapuino
USE_PMSIS_BSP = 1

ifeq ($(BOARD_NAME), gapuino)
APP_CFLAGS += -DHIMAX
endif

clean::
	$(RM) pics/*.pgm

ifeq ($(CAMERA), 1)
APP_CFLAGS += -DCAMERA=1
endif

AUTOTILER=1
ifeq ($(AUTOTILER), 1)
APP_SRCS += ResizeKernels.c $(RESIZE_KER_PATH)/ResizeBasicKernels.c
APP_INC += $(TILER_INC) $(RESIZE_KER_PATH)
APP_CFLAGS += -DAUTOTILER=1

RESIZE_MODEL_SRC = resize_model.c
RESIZE_MODEL_GEN = ResizeKernels
RESIZE_MODEL_GEN_C = $(addsuffix .c, $(RESIZE_MODEL_GEN))
RESIZE_MODEL_GEN_CLEAN = $(RESIZE_MODEL_GEN_C) $(addsuffix .h, $(RESIZE_MODEL_GEN))

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenResize -I$(TILER_INC) -I$(RESIZE_GEN_PATH) resize_model.c $(RESIZE_GEN_PATH)/ResizeGenerator.c $(TILER_LIB)

# Run the code generator
$(RESIZE_MODEL_GEN_C): GenTile
	./GenResize

model: $(RESIZE_MODEL_GEN_C)
	@mkdir -p pics

clean::
	$(RM) GenResize $(RESIZE_MODEL_GEN_CLEAN)

.PHONY: model clean
endif

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
