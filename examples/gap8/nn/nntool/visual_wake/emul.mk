# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

MODEL_PREFIX = vww
AT_INPUT_WIDTH=238
AT_INPUT_HEIGHT=208
AT_INPUT_COLORS=3

EMUL_MAIN=vww_emul
QUANT_BITS = 8
MODEL_SQ8 = 1
MODEL_SUFFIX=_SQ8BIT_EMUL

$(info Building emulation mode with 8 bit quantization)

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit

NNTOOL_SCRIPT=model/nntool_script_emul

TARGET_L1_SIZE = 64000
TARGET_L2_SIZE = 350000
include ../common/model_decl.mk
TRAINED_MODEL=model/visual_wake_quant.tflite

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
NNTOOL_EXTRA_FLAGS= -q

CC = gcc
CFLAGS += -g -m32 -O1 -D__EMUL__ -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS) -DPERF
INCLUDES = -I. -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD) -I$(MODEL_COMMON_INC)
LFLAGS =
LIBS = -lm
SRCS = $(EMUL_MAIN).c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
$(info CNN_LIB++ $(CNN_LIB))
$(info SRCS++ $(SRCS))
BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = $(MODEL_PREFIX)_emul

all: model $(EMUL_MAIN)

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
