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
TRAINED_MODEL=model/expressions.tflite
MODEL_SUFFIX = _$(QUANT_BITS)BIT_EMUL

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
