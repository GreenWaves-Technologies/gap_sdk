# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

MODEL_PREFIX = expressions
pulpChip = GAP
RM=rm -f

INPUT_1=$(CURDIR)/samples/input_0.dat
INPUT_2=$(CURDIR)/samples/input_1.dat
INPUT_3=$(CURDIR)/samples/input_2.dat
INPUT_4=$(CURDIR)/samples/input_3.dat

MODEL_SQ8=1
QUANT_BITS=8

NNTOOL_SCRIPT_PARAMETRIC = model/nntool_script_params_emul
NNTOOL_SCRIPT=model/nntool_script_emul
TRAINED_TFLITE_MODEL=model/expressions.tflite
MODEL_SUFFIX = _$(QUANT_BITS)BIT_EMUL

CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=350000
MODEL_L3_MEMORY=6388608

include common/model_decl.mk

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
MODEL_GENFLAGS_EXTRA+=
CC = gcc
CFLAGS += -g -m32 -O0 -D__EMUL__ -DINPUT_1=$(INPUT_1) -DINPUT_2=$(INPUT_2) -DINPUT_3=$(INPUT_3) -DINPUT_4=$(INPUT_4)
INCLUDES = -I. -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD) -I$(MODEL_COMMON_INC)
LFLAGS =
LIBS =
SRCS = $(MODEL_PREFIX).c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB) $(MODEL_BUILD)/Expression_Kernels.c
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

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(MAIN)

.PHONY: depend clean
include common/model_rules.mk
