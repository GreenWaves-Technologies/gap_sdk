# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

include common.mk

io=host

MEAS?=0
QUANT_BITS=8
#BUILD_DIR=BUILD
MODEL_SQ8=1

$(info Building $(TARGET_CHIP_FAMILY) mode with $(QUANT_BITS) bit quantization)

NNTOOL_SCRIPT=model/nntool_script_lprnet
TRAINED_TFLITE_MODEL=model/$(MODEL_PREFIX_LPR).tflite
MODEL_PREFIX=$(MODEL_PREFIX_LPR)
MODEL_SUFFIX=_LPR
IMAGE=$(CURDIR)/images/china_1_cropped.ppm
MAIN=$(MODEL_PREFIX_LPR).c

MODEL_QUANTIZED = 1

include common/model_decl.mk

MODEL_GENFLAGS_EXTRA+= 
NNTOOL_EXTRA_FLAGS += --use_hard_sigmoid
# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!

TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
	FREQ_CL?=50
	FREQ_FC?=50
	MODEL_L1_MEMORY=$(shell expr 125000 \- $(TOTAL_STACK_SIZE))
	MODEL_L2_MEMORY=1200000
	MODEL_L3_MEMORY=8388608
else
	ifeq '$(TARGET_CHIP)' 'GAP8_V3'
		FREQ_CL?=175
	else
		FREQ_CL?=50
	endif
	FREQ_FC?=250
	MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
	MODEL_L2_MEMORY=220000
	MODEL_L3_MEMORY=8388608
endif
# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

APP = OCRssd
APP_SRCS += $(MAIN) $(MODEL_COMMON_SRCS) $(CNN_LIB)

APP_CFLAGS += -O3
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) 
APP_SRCS += BUILD_MODEL_LPR/lprnetKernels.c
APP_CFLAGS += -IBUILD_MODEL_LPR


ifeq ($(MEAS),1)
	APP_CFLAGS += -DSILENT -DMEASUREMENTS
else
	APP_CFLAGS += -DPERF #-DHAVE_LCD 
endif
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE) -DFREQ_CL=$(FREQ_CL) -DFREQ_FC=$(FREQ_FC)
APP_CFLAGS += -DAT_INPUT_HEIGHT_SSD=$(AT_INPUT_HEIGHT_SSD) -DAT_INPUT_WIDTH_SSD=$(AT_INPUT_WIDTH_SSD) -DAT_INPUT_COLORS_SSD=$(AT_INPUT_COLORS_SSD)
APP_CFLAGS += -DAT_INPUT_HEIGHT_LPR=$(AT_INPUT_HEIGHT_LPR) -DAT_INPUT_WIDTH_LPR=$(AT_INPUT_WIDTH_LPR) -DAT_INPUT_COLORS_LPR=$(AT_INPUT_COLORS_LPR)

READFS_FILES=$(abspath $(MODEL_TENSORS))
PLPBRIDGE_FLAGS += -f

# all depends on the model
all:: model

clean:: #clean_model

at_model_disp:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS) --debug=Disp

at_model:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)

include common/model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
include $(RULES_DIR)/pmsis_rules.mk

