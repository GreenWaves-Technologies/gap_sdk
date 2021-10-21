# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

MODEL_PREFIX=kws
ifndef KWS_BITS
  KWS_BITS=16
endif

io=host

$(info Building GAP8 mode with $(KWS_BITS) bit quantization)

# For debugging don't load an image
# Run the network with zeros
# NO_IMAGE=1

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(KWS_BITS),8)
  $(info Configure 8 bit model)
  GAP_FLAGS += -DKWS_8BIT -DPRINT_IMAGE
  NNTOOL_SCRIPT=model/nntool_script8
  MODEL_SUFFIX = _8BIT
else
  ifeq ($(KWS_BITS),16)
    $(info Configure 16 bit model)
    GAP_FLAGS += -DKWS_16BIT
    NNTOOL_SCRIPT=model/nntool_script16
    MODEL_SUFFIX = _16BIT
  else
    $(error Don\'t know how to build with this bit width)
  endif
endif

include model_decl.mk

# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
MODEL_L1_MEMORY=48000
MODEL_L2_MEMORY=307200
MODEL_L3_MEMORY=1000000
# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

# use a custom template to switch on the performance checking
#MODEL_GENFLAGS_EXTRA= -c "model/code_template.c"

pulpChip = GAP
PULP_APP = kws2

APP_SRCS += kws.c $(MODEL_SRCS) $(CNN_LIB) 

APP_CFLAGS += -O3 -s -mno-memcpy -fno-tree-loop-distribute-patterns 
APP_CFLAGS += -I. -I./helpers -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(GEN_PATH) -I$(MODEL_BUILD) $(CNN_LIB_INCLUDE)
APP_CFLAGS += -DPERF

#APP_LDFLAGS +=  -lgaplib

READFS_FILES=$(realpath $(MODEL_TENSORS))
PLPBRIDGE_FLAGS = -f

# all depends on the model and the image header
all:: model 

clean:: clean_model

clean_all: clean clean_train
	rm -rf BUILD*
	rm kws_emul

.PHONY: clean_all

include model_rules.mk
include $(RULES_DIR)/pmsis_rules.mk
