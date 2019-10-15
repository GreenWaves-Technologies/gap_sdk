# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

MODEL_PREFIX=Mnist
ifndef MNIST_BITS
  MNIST_BITS=8
endif

# LINK_IMAGE=samples/5223_5.pgm

$(info Building emulation mode with $(MNIST_BITS) bit quantization)

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(MNIST_BITS),8)
  CFLAGS += -DMNIST_8BIT
  NNTOOL_SCRIPT=model/nntool_script_emul8
  MODEL_SUFFIX = _EMUL8BIT
else
  ifeq ($(MNIST_BITS),16)
    CFLAGS += -DMNIST_16BIT
    NNTOOL_SCRIPT=model/nntool_script_emul16
    MODEL_SUFFIX = _EMUL16BIT
  else
    $(error Dont know how to build with this bit width)
  endif
endif

include model_decl.mk

ifdef LINK_IMAGE
  LINK_IMAGE_HEADER=$(MODEL_BUILD)/image.h
  LINK_IMAGE_NAME=$(subst .,_,$(subst /,_,$(LINK_IMAGE)))
  GAP_FLAGS += -DLINK_IMAGE_HEADER="\"$(LINK_IMAGE_HEADER)\"" -DLINK_IMAGE_NAME="$(LINK_IMAGE_NAME)"
else
  LINK_IMAGE_HEADER=
endif

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
CC = gcc
CFLAGS += -g -O0 -D__EMUL__
INCLUDES = -I. -Ii./helpers -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(GEN_PATH) -I$(MODEL_BUILD)
LFLAGS =
LIBS =
SRCS = Mnist.c ImgIO.c helpers/helpers.c $(MODEL_SRCS)

BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = mnist_emul

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

all: model $(MAIN)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

ifdef LINK_IMAGE
$(LINK_IMAGE_HEADER): $(LINK_IMAGE)
	xxd -i $< $@
endif

clean: clean_model
	$(RM) -r $(BUILD_DIR)
	$(RM) $(MAIN)

.PHONY: depend clean

include model_rules.mk
