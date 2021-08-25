# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

MODEL_SUFFIX?=

MODEL_PREFIX?=

MODEL_PYTHON=python3

MODEL_PATH = $(MODEL_BUILD)/$(MODEL_PREFIX)$(suffix $(TRAINED_MODEL))
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
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN)) $(MODEL_EXPRESSIONS)
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN)) $(MODEL_EXPRESSIONS)
MODEL_GEN_EXE = $(MODEL_BUILD)/GenTile

ifdef MODEL_QUANTIZED
  NNTOOL_EXTRA_FLAGS = -q
endif

MODEL_GENFLAGS_EXTRA?=
EXTRA_GENERATOR_SRC?=
NNTOOL_SCRIPT?=

$(info script $(NNTOOL_SCRIPT))
RM=rm -f

NNTOOL?=nntool

TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr $(TARGET_L1_SIZE) \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=$(TARGET_L2_SIZE)
MODEL_L3_MEMORY=$(TARGET_L3_SIZE)

# hram - HyperBus RAM
# qspiram - Quad SPI RA
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_EXEC?=hram
$(info GEN ... $(CNN_GEN))
