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

LINK_IMAGE=images/features_0_1.pgm


$(info Building GAP8 mode with $(KWS_BITS) bit quantization)

# For debugging don't load an image
# Run the network with zeros
# NO_IMAGE=1

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(KWS_BITS),8)
  $(info Configure 8 bit model)
  MODEL_SQ8 = 1
  APP_CFLAGS += -DKWS_8BIT -DPRINT_IMAGE
  NNTOOL_SCRIPT=model/nntool_script8
  MODEL_SUFFIX = _8BIT
else
  ifeq ($(KWS_BITS),16)
    # use a custom template to switch on the performance checking
    MODEL_GENFLAGS_EXTRA= -c "model/code_template.c"
    $(info Configure 16 bit model)
    APP_CFLAGS += -DKWS_16BIT
    NNTOOL_SCRIPT=model/nntool_script16
    MODEL_SUFFIX = _16BIT
  else
    $(error Don\'t know how to build with this bit width)
  endif
endif

include model_decl.mk

ifdef LINK_IMAGE
  LINK_IMAGE_HEADER=$(MODEL_BUILD)/image.h
  LINK_IMAGE_NAME=$(subst .,_,$(subst /,_,$(LINK_IMAGE)))
  APP_CFLAGS += -DLINK_IMAGE_HEADER="\"$(LINK_IMAGE_HEADER)\"" -DLINK_IMAGE_NAME="$(LINK_IMAGE_NAME)"
else
  LINK_IMAGE_HEADER=
endif

# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
MODEL_L1_MEMORY=52000
MODEL_L2_MEMORY=307200
MODEL_L3_MEMORY=8388608
# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

pulpChip = GAP
PULP_APP = kws2
USE_PMSIS_BSP=1

APP_SRCS += kws.c ImgIO.c $(MODEL_GEN_C) $(CNN_LIB) #MFCC_Dump.c ./model/layers.c 

APP_CFLAGS += -O3 -s -mno-memcpy -fno-tree-loop-distribute-patterns -Wno-sign-compare -Wno-maybe-uninitialized -Wno-type-limits
APP_CFLAGS += -I. -I./helpers -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD)
APP_CFLAGS += #-DPERF

ifneq ($(platform),gvsoc)
  $(info Platform is GAPUINO)
  PLPBRIDGE_FLAGS = -f $(MODEL_TENSORS)
endif

READFS_FILES = $(realpath $(MODEL_TENSORS))

ifdef NO_IMAGE
  APP_CFLAGS += -DNO_IMAGE
else
  ifndef LINK_IMAGE
    PLPBRIDGE_FLAGS += -fileIO 15
  endif
endif

ifdef LINK_IMAGE
# all depends on the model and the image header
all:: model $(LINK_IMAGE_HEADER)

$(LINK_IMAGE_HEADER): $(LINK_IMAGE)
	xxd -i $< $@
else
# all depends on the model
all:: model
endif

clean:: clean_model

clean_all: clean clean_train
	rm -rf BUILD*
	rm kws_emul

.PHONY: clean_all

#PMSIS_OS = freertos
io = host
include model_rules.mk
include $(RULES_DIR)/pmsis_rules.mk

