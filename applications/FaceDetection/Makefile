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

APP = face_detection

NB_FRAMES ?= -1

FACE_DET_MODEL_SRC = FaceDetGenerator.c FaceDetModel.c
FACE_DET_MODEL_GEN = FaceDetKernels
FACE_DET_MODEL_GEN_C = $(addsuffix .c, $(FACE_DET_MODEL_GEN))
FACE_DET_MODEL_GEN_CLEAN = $(FACE_DET_MODEL_GEN_C) $(addsuffix .h, $(FACE_DET_MODEL_GEN))
FACE_DET_SRCS +=  main.c faceDet.c FaceDetBasicKernels.c ImageDraw.c $(FACE_DET_MODEL_GEN_C)

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm

APP_SRCS += $(FACE_DET_SRCS)
APP_INC  += $(TILER_EMU_INC) $(TILER_INC)
APP_CFLAGS += -O3 -g -D__PMSIS__ -DUSE_CAMERA -DNB_FRAMES=$(NB_FRAMES)

display ?= 1
ifeq ($(display), 1)
APP_CFLAGS += -DUSE_DISPLAY
endif

BOARD_NAME ?= gapoc_a
PMSIS_OS ?= freertos
USE_PMSIS_BSP=1

export GAP_USE_OPENOCD=1
io=host

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
