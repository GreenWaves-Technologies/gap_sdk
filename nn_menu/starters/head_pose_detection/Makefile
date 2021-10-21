# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

QUANT_FLAG = 

io=host

QUANT_BITS=8
BUILD_DIR=BUILD
$(info Building GAP8 mode with $(QUANT_BITS) bit quantization)

MODEL_SQ8=1 # use scale based quantization (tflite-like)

QUANT_TFLITE?=1
ifeq ($(QUANT_TFLITE), 0)
	MODEL_PREFIX = vww17
	NNTOOL_SCRIPT?=models/nntool_script
	TRAINED_TFLITE_MODEL=models/$(MODEL_PREFIX).tflite
else
	MODEL_PREFIX = vww17
	NNTOOL_SCRIPT?=models/nntool_script_quant_tflite
	TRAINED_TFLITE_MODEL=models/$(MODEL_PREFIX)_new_samples.tflite
	QUANT_FLAG = -q
endif
MODEL_SUFFIX = _SQ8BIT
AT_INPUT_HEIGHT = 128
AT_INPUT_WIDTH = 128

IMAGE=$(CURDIR)/images/ofilm/looking/20cm-6.ppm

GALAXY_CORE?=0
ifeq ($(GALAXY_CORE), 1)
	INPUT_WIDTH=320
	INPUT_HEIGHT=240
else
	INPUT_WIDTH=324
	INPUT_HEIGHT=244
endif
ifeq ($(platform), gvsoc)
	INPUT_WIDTH=320
	INPUT_HEIGHT=240
endif

include common/model_decl.mk

# Here we set the default memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
CLUSTER_STACK_SIZE?=4028
CLUSTER_SLAVE_STACK_SIZE?=1024
TOTAL_STACK_SIZE?=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY?=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY?=250000
MODEL_L3_MEMORY?=8388608
ifeq '$(TARGET_CHIP)' 'GAP8_V3'
	FREQ_CL?=175
else
	FREQ_CL?=50
endif
FREQ_FC?=150

# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

pulpChip = GAP
PULP_APP = headpose
USE_PMSIS_BSP=1

APP = headpose
MAIN ?= main.c
APP_SRCS += $(MAIN) $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB) $(TILER_GENERATOR_PATH)/BilinearResizes/ResizeBasicKernels.c

APP_CFLAGS += -g -O3 -mno-memcpy -fno-tree-loop-distribute-patterns
# list of includes file
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(realpath $(MODEL_BUILD)) -I$(TILER_GENERATOR_PATH)/BilinearResizes
# pass also macro defines to the compiler
APP_CFLAGS += -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE) -DMODEL_ID=$(MODEL_ID) -DFREQ_FC=$(FREQ_FC) -DFREQ_CL=$(FREQ_CL)
APP_CFLAGS += -DINPUT_WIDTH=$(INPUT_WIDTH) -DINPUT_HEIGHT=$(INPUT_HEIGHT) $(MODEL_SIZE_CFLAGS)

ifeq ($(platform), gvsoc)
	APP_CFLAGS += -DHAVE_LCD -DPERF
else
	APP_CFLAGS += -DSILENT -DHAVE_HIMAX -DHAVE_LCD
endif

# this line is needed to flash into the chip the model tensors 
# and other constants needed by the Autotiler
READFS_FILES=$(abspath $(MODEL_TENSORS))

# This are the probability threshold after a sigmoid activation
# They need to be converted as the fixed point number of the sigmoid 
# input to be compared to our model output
HEAD_THR = 0.1
LOOK_THR = 0.68
compute_thresholds:
	python utils/compute_thresholds.py --head_thr $(HEAD_THR) --look_thr $(LOOK_THR)

# all depends on the model
all:: model compute_thresholds

clean:: clean_model

include common/model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
include $(RULES_DIR)/pmsis_rules.mk

