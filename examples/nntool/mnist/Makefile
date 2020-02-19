# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

io=host

include common.mk

QUANT_BITS ?= 8

$(info Building GAP8 mode with $(QUANT_BITS) bit quantization)

# For debugging don't load an image
# Run the network with zeros
# NO_IMAGE=1

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(QUANT_BITS),8)
  $(info Configure 8 bit model)
  APP_CFLAGS += -DQUANT_8BIT
  NNTOOL_SCRIPT=model/nntool_script8
  MODEL_SUFFIX = _8BIT
else
  ifeq ($(QUANT_BITS),16)
    $(info Configure 16 bit model)
    APP_CFLAGS += -DQUANT_16BIT
    NNTOOL_SCRIPT=model/nntool_script16
    MODEL_SUFFIX = _16BIT
  else
    $(error Don\'t know how to build with this bit width)
  endif
endif

include ../common/model_decl.mk
include common_rules.mk

# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
CLUSTER_STACK_SIZE=2048
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=370000
MODEL_L3_MEMORY=8388608

# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

# use a custom template to switch on the performance checking
MODEL_GENFLAGS_EXTRA=

pulpChip = GAP
APP = mnist2
USE_PMSIS_BSP=1

APP_SRCS += $(MODEL_PREFIX).c $(MODEL_COMMON_SRCS) $(MODEL_SRCS)

APP_CFLAGS += -g -O1 -mno-memcpy -fno-tree-loop-distribute-patterns -DDONT_DUMP
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(TILER_CNN_KERNEL_PATH) -I$(MODEL_BUILD)
APP_CFLAGS += -DPERF $(MODEL_SIZE_CFLAGS) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE) -DSTACK_SIZE=$(CLUSTER_STACK_SIZE)

READFS_FILES=$(abspath $(MODEL_TENSORS))
PLPBRIDGE_FLAGS += -f

ifdef NO_IMAGE
  APP_CFLAGS += -DNO_IMAGE
endif

# all depends on the model
all:: model

clean:: clean_model

clean_all: clean clean_train
	rm -rf BUILD*
	rm mnist_emul

.PHONY: clean_all

include ../common/model_rules.mk
include $(RULES_DIR)/pmsis_rules.mk

