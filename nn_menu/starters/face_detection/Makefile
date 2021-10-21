# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

MODEL_PREFIX=face_detection

ifndef QUANTIZATION_BITS
  QUANTIZATION_BITS=16
endif

$(info Building GAP8 mode with $(QUANTIZATION_BITS) bit quantization)

## Mute printf in source code
#SILENT=1

## Enable image grub from camera and disaply output to lcd
#FROM_CAMERA=1

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(QUANTIZATION_BITS),8)
  MODEL_SQ8=1
  GAP_FLAGS += -DQUANTIZATION_8BIT
  NNTOOL_SCRIPT=model/nntool_script8
  MODEL_SUFFIX = _8BIT
else
  ifeq ($(QUANTIZATION_BITS),16)
    MODEL_POW2=1
    GAP_FLAGS += -DQUANTIZATION_16BIT
    NNTOOL_SCRIPT=model/nntool_script16
    MODEL_SUFFIX = _16BIT
  else
    $(error Don't know how to build with this bit width)
  endif
endif

include model_decl.mk

# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
# FC stack size:
MAIN_STACK_SIZE=2048
# Cluster PE0 stack size:
CLUSTER_STACK_SIZE=4096
# Cluster PE1-PE7 stack size:
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
  FREQ_CL?=50
  FREQ_FC?=50
  MODEL_L1_MEMORY=$(shell expr 125000 \- $(TOTAL_STACK_SIZE))
  MODEL_L2_MEMORY=1300000
  MODEL_L3_MEMORY=8388608
else
  ifeq '$(TARGET_CHIP)' 'GAP8_V3'
    FREQ_CL?=175
  else
    FREQ_CL?=50
  endif
  FREQ_FC?=250
  MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
  MODEL_L2_MEMORY=150000
  MODEL_L3_MEMORY=8388608
endif
# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

APP=face_detection
APP_SRCS += main.c ImgIO.c ImageDraw.c SSDKernels.c SSDBasicKernels.c SSDParams.c $(MODEL_SRCS) $(CNN_LIB)

APP_CFLAGS += -w -O2 -s -mno-memcpy -fno-tree-loop-distribute-patterns
APP_CFLAGS += -I. -I./helpers $(CNN_LIB_INCLUDE) -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(GEN_PATH) -I$(MODEL_BUILD)
APP_CFLAGS += -DCLUSTER_STACK_SIZE=$(CLUSTER_STACK_SIZE) -DCLUSTER_SLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)

ifeq ($(SILENT),1)
  APP_CFLAGS += -DSILENT=1
endif
ifeq ($(FROM_CAMERA),1)
  APP_CFLAGS += -DFROM_CAMERA=1
endif

ifeq ($(platform),gvsoc)
  $(info Platform is GVSOC)
  READFS_FILES=$(MODEL_TENSORS)
else
  PLPBRIDGE_FLAGS = -f
  READFS_FILES=$(MODEL_TENSORS)
endif

io=host

#####Here we add cutom kernels that are not yet integrated into AT libraries

SSD_MODEL_GEN = SSDKernels
SSD_MODEL_GEN_C = $(addsuffix .c, $(SSD_MODEL_GEN))
SSD_MODEL_GEN_CLEAN = $(SSD_MODEL_GEN_C) $(addsuffix .h, $(SSD_MODEL_GEN))

GenSSDTile: SSDModel.c
	gcc -g -o GenSSDTile -I$(TILER_EMU_INC) -I"$(TILER_INC)" SSDModel.c $(TILER_LIB) #-lSDL2 -lSDL2_ttf -DAT_DISPLAY

$(SSD_MODEL_GEN_C): GenSSDTile
	./GenSSDTile

SSDParams.c: $(SSD_MODEL_GEN_C)
	cd SSDParamsGenerator && $(MAKE) all run

SSD_model: SSDParams.c

#################


# all depends on the model
all:: SSD_model model

clean_at_model:
	rm -rf BUILD
	rm -rf GenTile

clean:: #clean_model
	rm -rf BUILD
	rm -rf GenTile
	rm -rf $(MODEL_BUILD)/face_detectionKernels.*
	cd SSDParamsGenerator && $(MAKE) clean
	rm -rf SSDParams.c SSDParams.h
	rm -rf GenSSDTile $(SSD_MODEL_GEN_CLEAN)

clean_all: clean clean_model #clean_train


.PHONY: clean_all

include model_rules.mk
include $(RULES_DIR)/pmsis_rules.mk
