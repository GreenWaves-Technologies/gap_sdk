# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

RM=rm -f

APP = face_detection

FACE_DET_MODEL_SRC = FaceDetGenerator.c FaceDetModel.c
FACE_DET_MODEL_GEN = FaceDetKernels
FACE_DET_MODEL_GEN_C = $(addsuffix .c, $(FACE_DET_MODEL_GEN))
FACE_DET_MODEL_GEN_CLEAN = $(FACE_DET_MODEL_GEN_C) $(addsuffix .h, $(FACE_DET_MODEL_GEN))
FACE_DET_SRCS +=  main.c faceDet.c FaceDetBasicKernels.c ImageDraw.c $(FACE_DET_MODEL_GEN_C)

APP_SRCS += $(FACE_DET_SRCS)
APP_INC  += $(TILER_INC)
APP_CFLAGS += -O3 -g -D__PMSIS__ -DUSE_CAMERA -DUSE_DISPLAY

BOARD_NAME ?= gapoc_a
PMSIS_OS ?= freertos
USE_PMSIS_BSP=1

ifeq '$(BOARD_NAME)' 'gapuino'
APP_CFLAGS += -DGAPUINO
APP_CFLAGS += -DHIMAX
endif

ifeq '$(BOARD_NAME)' 'gapoc_a'
PLPBRIDGE_FLAGS += -ftdi
APP_CFLAGS += -DGAPOC_A
endif

#PLPBRIDGE_FLAGS += -jtag

all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) $(FACE_DET_MODEL_SRC) $(TILER_LIB) $(APP_CFLAGS)

# Run the code generator
$(FACE_DET_MODEL_GEN_C): GenTile
	./GenTile

model: $(FACE_DET_MODEL_GEN_C)

clean::
	$(RM) GenTile $(FACE_DET_MODEL_GEN_CLEAN)

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
