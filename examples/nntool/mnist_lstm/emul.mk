# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

include common.mk

QUANT_BITS?=8
BUILD_DIR=BUILD
MODEL_SQ8=1
NNTOOL_SCRIPT=model/nntool_script_emul
MODEL_SUFFIX = _SQ8BIT_EMUL
TRAINED_TFLITE_MODEL=model/rnn.tflite
RAM_FLASH_TYPE ?= HYPER

ifeq '$(RAM_FLASH_TYPE)' 'HYPER'
CFLAGS += -DUSE_HYPER
MODEL_L3_EXEC=hram
MODEL_L3_CONST=hflash
else
CFLAGS += -DUSE_SPI
CONFIG_SPIRAM = 1
MODEL_L3_EXEC=qspiram
MODEL_L3_CONST=qpsiflash
endif

include ../common/model_decl.mk
include common_rules.mk

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
MODEL_GENFLAGS_EXTRA+=
CC = gcc
CFLAGS += -g -m32 -O0 -D__EMUL__ -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS) -DPERF
INCLUDES = -I. -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD) -I$(MODEL_COMMON_INC)
LFLAGS =
LIBS =
SRCS = $(MODEL_PREFIX)_emul.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
$(info CNN_LIB++ $(CNN_LIB))
$(info SRCS++ $(SRCS))
BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = $(MODEL_PREFIX)_emul
# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
CLUSTER_STACK_SIZE=2048
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=200000
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
