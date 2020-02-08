# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

include common.mk

QUANT_BITS?=8
MODEL_SUFFIX=_$(QUANT_BITS)BIT_EMUL

# LINK_IMAGE=samples/5223_5.pgm

$(info Building emulation mode with $(QUANT_BITS) bit quantization)

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(QUANT_BITS),8)
  CFLAGS += -DQUANT_8BIT
  NNTOOL_SCRIPT=model/nntool_script_emul8
else
  ifeq ($(QUANT_BITS),16)
    CFLAGS += -DQUANT_16BIT
    NNTOOL_SCRIPT=model/nntool_script_emul16
  else
    $(error Dont know how to build with this bit width)
  endif
endif

include ../common/model_decl.mk
include common_rules.mk

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
CC = gcc
CFLAGS += -g -O0 -D__EMUL__ $(MODEL_SIZE_CFLAGS) -DPERF
INCLUDES = -I. -I./helpers -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(TILER_CNN_GENERATOR_PATH) -I$(TILER_CNN_KERNEL_PATH) -I$(MODEL_BUILD) -I$(MODEL_COMMON_INC)
LFLAGS =
LIBS =
SRCS = $(MODEL_PREFIX).c $(MODEL_COMMON_SRCS) $(MODEL_SRCS)

BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = $(MODEL_PREFIX)_emul

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

clean: clean_model
	$(RM) -r $(BUILD_DIR)
	$(RM) $(MAIN)

.PHONY: depend clean

include ../common/model_rules.mk
