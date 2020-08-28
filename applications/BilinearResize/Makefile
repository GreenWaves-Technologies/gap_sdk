# Copyright 2019 GreenWaves Technologies, SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

RM=rm -f

RESIZE_GEN_PATH = $(TILER_BILINEAR_RESIZE_GENERATOR_PATH)
RESIZE_KER_PATH = $(TILER_BILINEAR_RESIZE_GENERATOR_PATH)

APP = test_resize
APP_SRCS += test_resize.c ImgIO.c
APP_INC += . $(TILER_EMU_INC) $(TILER_INC)
APP_CFLAGS +=

#PLPBRIDGE_FLAGS += -fileIO 2

PMSIS_OS ?= freertos
BOARD_NAME ?= gapuino
USE_PMSIS_BSP = 1

ifeq ($(BOARD_NAME), gapuino)
APP_CFLAGS += -DHIMAX
endif

export GAP_USE_OPENOCD=1
io=host

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
