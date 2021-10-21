# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

MODEL_PREFIX = squeezenet
MODEL_SQ8=1
AT_INPUT_WIDTH=224
AT_INPUT_HEIGHT=224
AT_INPUT_COLORS=3
RM=rm -r

QUANT_BITS = 8
MODEL_SUFFIX=_$(QUANT_BITS)BIT_EMUL

$(info Building emulation mode with 8 bit quantization)

TRAINED_TFLITE_MODEL = model/squeezenet.tflite
NNTOOL_SCRIPT=model/nntool_script_emul

CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=350000
MODEL_L3_MEMORY=6388608
MODEL_SIZE_CFLAGS = -DAT_INPUT_HEIGHT=$(AT_INPUT_HEIGHT) -DAT_INPUT_WIDTH=$(AT_INPUT_WIDTH) -DAT_INPUT_COLORS=$(AT_INPUT_COLORS)

include common/model_decl.mk

MODEL_GEN_EXTRA_FLAGS = -f $(MODEL_BUILD)
CC = gcc
CFLAGS += -g -O0 -D__EMUL__ -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS) 
INCLUDES = -I. -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(TILER_CNN_GENERATOR_PATH) -I$(TILER_CNN_KERNEL_PATH) -I$(MODEL_BUILD) -I$(MODEL_COMMON_INC) $(CNN_LIB_INCLUDE)
LFLAGS =
LIBS =
SRCS = $(MODEL_PREFIX).c $(MODEL_COMMON_SRCS) $(MODEL_GEN_C) $(CNN_LIB)

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
	rm -rf $(BUILD_DIR)
	$(RM) $(MAIN)

clean_at_model:
	$(RM) $(MODEL_GEN_EXE)

at_model_disp:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS) --debug=Disp

at_model:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)

.PHONY: depend clean

include common/model_rules.mk
