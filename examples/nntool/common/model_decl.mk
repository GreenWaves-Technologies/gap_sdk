# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

MODEL_SUFFIX?=

MODEL_PREFIX?=

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(QUANT_BITS),8)
  TRAIN_SUFFIX=_8BIT
else
  ifeq ($(QUANT_BITS),16)
    TRAIN_SUFFIX=_16BIT
  else
    $(error Dont know how to build with this bit width)
  endif
endif

MODEL_PYTHON=python3

ifeq ($(ONNX), 1)
  TRAINED_MODEL ?= model/$(MODEL_PREFIX).onnx
  MODEL_PATH     = $(MODEL_BUILD)/$(MODEL_PREFIX).onnx
else
  TRAINED_MODEL ?= model/$(MODEL_PREFIX).tflite
  MODEL_PATH     = $(MODEL_BUILD)/$(MODEL_PREFIX).tflite
endif
MODEL_COMMON ?= common
MODEL_COMMON_INC ?= $(GAP_SDK_HOME)/libs/gap_lib/include
MODEL_COMMON_SRC ?= $(GAP_SDK_HOME)/libs/gap_lib/img_io
MODEL_COMMON_SRC_FILES ?= ImgIO.c
MODEL_COMMON_SRCS = $(realpath $(addprefix $(MODEL_COMMON_SRC)/,$(MODEL_COMMON_SRC_FILES)))
MODEL_BUILD = BUILD_MODEL$(MODEL_SUFFIX)

TENSORS_DIR = $(MODEL_BUILD)/tensors
MODEL_TENSORS = $(MODEL_BUILD)/$(MODEL_PREFIX)_L3_Flash_Const.dat

MODEL_STATE = $(MODEL_BUILD)/$(MODEL_PREFIX).json
MODEL_SRC = $(MODEL_PREFIX)Model.c
MODEL_HEADER = $(MODEL_PREFIX)Info.h
MODEL_GEN = $(MODEL_BUILD)/$(MODEL_PREFIX)Kernels 
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
MODEL_GEN_EXE = $(MODEL_BUILD)/GenTile

ifdef MODEL_QUANTIZED
  NNTOOL_EXTRA_FLAGS = -q
endif

MODEL_GENFLAGS_EXTRA =

EXTRA_GENERATOR_SRC =

$(info script $(NNTOOL_SCRIPT))
ifndef NNTOOL_SCRIPT
  NNTOOL_SCRIPT=model/nntool_script
endif
IMAGES ?= images
RM=rm -f

NNTOOL=nntool

# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
MODEL_L1_MEMORY=52000
MODEL_L2_MEMORY=307200
MODEL_L3_MEMORY=8388608
# hram - HyperBus RAM
# qspiram - Quad SPI RA
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash

include $(RULES_DIR)/at_common_decl.mk
$(info GEN ... $(CNN_GEN))
