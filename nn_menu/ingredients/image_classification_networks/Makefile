# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

include common.mk
QUANT_FLAG ?= -q

ifeq ($(AT_INPUT_WIDTH), 224)
	IMAGE=$(CURDIR)/images/ILSVRC2012_val_00011158_224.ppm
endif
ifeq ($(AT_INPUT_WIDTH), 192)
	IMAGE=$(CURDIR)/images/ILSVRC2012_val_00011158_192.ppm
endif
ifeq ($(AT_INPUT_WIDTH), 160)
	IMAGE=$(CURDIR)/images/ILSVRC2012_val_00011158_160.ppm
endif
ifeq ($(AT_INPUT_WIDTH), 128)
	IMAGE=$(CURDIR)/images/ILSVRC2012_val_00011158_128.ppm
endif
ifeq ($(AT_INPUT_WIDTH), 96)
	IMAGE=$(CURDIR)/images/ILSVRC2012_val_00011158_96.ppm
endif

io=host

QUANT_BITS=8
BUILD_DIR=BUILD
$(info Building GAP8 mode with $(QUANT_BITS) bit quantization)

MODEL_SQ8=1 # use scale based quantization (tflite-like)

NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script
MODEL_SUFFIX=_SQ8BIT
TRAINED_TFLITE_MODEL=models/tflite_models/$(MODEL_PREFIX).tflite

include common/model_decl.mk

# Here we set the default memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
CLUSTER_STACK_SIZE?=6096
CLUSTER_SLAVE_STACK_SIZE?=1024
TOTAL_STACK_SIZE = $(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
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
	MODEL_L2_MEMORY?=300000
	MODEL_L3_MEMORY=8000000
endif

# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash
# ram - Model input from ram
# l2  - Model input from l2 memory
MODEL_INPUT=ram


pulpChip = GAP
PULP_APP = imagenet
USE_PMSIS_BSP=1
PMSIS_OS?=pulpos

APP = imagenet
MAIN ?= main.c
APP_SRCS += $(MAIN) $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)

APP_CFLAGS += -g -O3 -mno-memcpy -fno-tree-loop-distribute-patterns
# list of includes file
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD) -I$(MODEL_HEADERS)
# pass also macro defines to the compiler
APP_CFLAGS += -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE) -DPERF -DMODEL_ID=$(MODEL_ID) -DFREQ_FC=$(FREQ_FC) -DFREQ_CL=$(FREQ_CL)
APP_CFLAGS += -DAT_CONSTRUCT=$(AT_CONSTRUCT) -DAT_DESTRUCT=$(AT_DESTRUCT) -DAT_CNN=$(AT_CNN) -DAT_L3_ADDR=$(AT_L3_ADDR)

HAVE_CAMERA?=0
HAVE_LCD?=0
ifeq ($(HAVE_CAMERA), 1)
	APP_CFLAGS += -DHAVE_CAMERA
endif
ifeq ($(HAVE_LCD), 1)
	APP_CFLAGS += -DHAVE_LCD
endif

ifeq '$(MODEL_L3_EXEC)' 'qspiram'
	MODEL_L3_RAM=AT_MEM_L3_QSPIRAM
	APP_CFLAGS += -DUSE_QSPI
else ifeq '$(MODEL_L3_EXEC)' 'hram'
	MODEL_L3_RAM=AT_MEM_L3_HRAM
else
	$(error MODEL_L3_EXEC can only be qspiram or hram)
endif

ifeq '$(MODEL_L3_CONST)' 'qpsiflash'
	MODEL_L3_FLASH=AT_MEM_L3_QSPIFLASH
else ifeq '$(MODEL_L3_CONST)' 'hflash'
	MODEL_L3_FLASH=AT_MEM_L3_HFLASH
else
	$(error MODEL_L3_CONST can only be qpsiflash or hflash)
endif

ifeq '$(MODEL_INPUT)' 'ram'
	ifeq '$(MODEL_L3_EXEC)' 'qspiram'
		MODEL_INPUT=AT_MEM_L3_QSPIRAM
	else
		MODEL_INPUT=AT_MEM_L3_HRAM
	endif
else ifeq '$(MODEL_INPUT)' 'l2'
	MODEL_INPUT=AT_MEM_L2
else
	$(error MODEL_INPUT can only be ram or l2)
endif

# this line is needed to flash into the chip the model tensors
# and other constants needed by the Autotiler
READFS_FILES=$(abspath $(MODEL_TENSORS))
PLPBRIDGE_FLAGS += -f

# all depends on the model
all:: model

clean:: clean_model

clean_at_model::
	$(RM) $(MODEL_GEN_C)

TFLITE_PYSCRIPT= models/tflite_inference.py
test_tflite:
	python $(TFLITE_PYSCRIPT) -t $(TRAINED_TFLITE_MODEL) -i $(IMAGE)

include common/model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
include $(RULES_DIR)/pmsis_rules.mk

