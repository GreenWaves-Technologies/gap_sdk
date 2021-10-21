# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

RGB?=0
TEST_ACC?=0
include common.mk

QUANT_BITS = 8
MODEL_SQ8 = 1
MODEL_SUFFIX=_SQ$(QUANT_BITS)BIT_EMUL

ifeq ($(TEST_ACC), 1)
	MAIN = main_accuracy.c
	ifeq ($(RGB), 1)
		NNTOOL_SCRIPT=model/nntool_script_rgb_acc
	else
		NNTOOL_SCRIPT=model/nntool_script_bw_acc
	endif
else
	MAIN = main_emul.c
	ifeq ($(RGB), 1)
		NNTOOL_SCRIPT=model/nntool_script_rgb_emul
	else
		NNTOOL_SCRIPT=model/nntool_script_bw_emul
	endif
endif

TRAINED_TFLITE_MODEL=model/trained_models/$(MODEL_PREFIX).tflite

include common/model_decl.mk

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
CC = gcc
CFLAGS += -g -m32 -O3 -D__EMUL__ -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS) -DSILENT
ifeq ($(RGB), 1)
	CFLAGS += -DRGB
endif
INCLUDES = -I. -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD) -I$(MODEL_COMMON_INC)
LFLAGS =
LIBS =
SRCS = $(MAIN) $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
$(info CNN_LIB++ $(CNN_LIB))
$(info SRCS++ $(SRCS))
BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

EXE = $(MODEL_PREFIX)_$(basename $(MAIN))
# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
CLUSTER_STACK_SIZE=2048
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=300000
MODEL_L3_MEMORY=8388608

# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

all: model $(EXE)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(EXE) $(OBJS) $(LFLAGS) $(LIBS)

clean: #clean_model
	$(RM) -r $(BUILD_DIR)
	$(RM) $(EXE)

clean_all: clean clean_model
	$(RM) -r $(BUILD_DIR)
	$(RM) -r *_emul
	$(RM) -r *_accuracy

.PHONY: depend clean

include common/model_rules.mk
