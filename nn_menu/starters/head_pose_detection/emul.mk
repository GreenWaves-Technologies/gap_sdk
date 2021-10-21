# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

io=host

QUANT_BITS=8
#BUILD_DIR=BUILD
MODEL_SQ8=1

$(info Building GAP8 mode with $(QUANT_BITS) bit quantization)

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
MODEL_SUFFIX=_SQ8BIT_EMUL
QUANT_TFLITE?=1
ifeq ($(QUANT_TFLITE), 0)
	MODEL_PREFIX = vww17
	NNTOOL_SCRIPT?=models/nntool_script_emul
	TRAINED_TFLITE_MODEL=models/$(MODEL_PREFIX).tflite
else
	MODEL_PREFIX = vww17
	NNTOOL_SCRIPT?=models/nntool_script_quant_tflite_emul
	TRAINED_TFLITE_MODEL=models/$(MODEL_PREFIX)_new_samples.tflite
	QUANT_FLAG = -q
endif

AT_INPUT_HEIGHT = 128
AT_INPUT_WIDTH = 128
ALREADY_RESIZED?=0

MODEL_QUANTIZED = 1

include common/model_decl.mk

CC = gcc
CFLAGS += -g -m32 -O0 -D__EMUL__ -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS) -DSILENT
CFLAGS += -I. -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(GAP_SDK_HOME)/libs/gap_lib/include -I$(MODEL_BUILD) -I$(TILER_GENERATOR_PATH)/BilinearResizes
ifeq ($(ALREADY_RESIZED), 1)
	CFLAGS += -DALREADY_RESIZED
endif
LFLAGS =
LIBS =
SRCS = main_emul.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB) $(TILER_GENERATOR_PATH)/BilinearResizes/ResizeBasicKernels.c
BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = $(MODEL_PREFIX)_emul
# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=300000
MODEL_L3_MEMORY=8388608

MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

# This are the probability threshold after a sigmoid activation
# They need to be converted as the fixed point number of the sigmoid 
# input to be compared to our model output
HEAD_THR = 0.1
LOOK_THR = 0.68
compute_thresholds:
	python utils/compute_thresholds.py --head_thr $(HEAD_THR) --look_thr $(LOOK_THR)

all:: model compute_thresholds $(MAIN)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

clean:: #clean_model
	$(RM) -r $(BUILD_DIR)
	$(RM) $(MAIN)

.PHONY: depend clean

include common/model_rules.mk
