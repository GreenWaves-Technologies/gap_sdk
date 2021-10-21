# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

include common.mk

io=host

QUANT_BITS=8
#BUILD_DIR=BUILD
MODEL_SQ8=1

$(info Building GAP8 mode with $(QUANT_BITS) bit quantization)

ifeq ($(MODEL),1)
	MODEL_PREFIX=$(MODEL_PREFIX_LPR)
	NNTOOL_SCRIPT=model/nntool_script_emul_lprnet
	TRAINED_TFLITE_MODEL=model/$(MODEL_PREFIX).tflite
	MODEL_SUFFIX = _LPR_EMUL
else
	MODEL_PREFIX=$(MODEL_PREFIX_SSD)
	NNTOOL_SCRIPT=model/nntool_script_emul_ssdlite
	TRAINED_TFLITE_MODEL=model/$(MODEL_PREFIX).tflite
	MODEL_SUFFIX = _SSD_EMUL
endif

MODEL_QUANTIZED = 1

include common/model_decl.mk

CC = gcc
CFLAGS += -g -m32 -O0 -D__EMUL__ -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD)
CFLAGS += $(AT_SIZES_SSD) $(AT_SIZES_LPR)
LFLAGS =
LIBS =
SRCS = $(MODEL_PREFIX).c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB) $(TILER_CNN_KERNEL_PATH)/SSD_BasicKernels.c
BUILD_DIR = BUILD_EMUL

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)

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

all:: model $(MAIN)

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
