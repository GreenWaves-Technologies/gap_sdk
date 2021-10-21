# Copyright (C) 2021 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.	See the LICENSE file for details.

ifndef GAP_SDK_HOME
	$(error Source sourceme in gap_sdk first)
endif

include common.mk

MODEL_PREFIX=MCUNet

RAM_FLASH_TYPE ?= HYPER
#PMSIS_OS=freertos

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

$(info Building NNTOOL model)
NNTOOL_EXTRA_FLAGS ?= 

include common/model_decl.mk

TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=45000 #$(shell expr $(TARGET_L1_SIZE) \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=200000
MODEL_L3_MEMORY=$(TARGET_L3_SIZE)

# IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
#################################################
CC = gcc

SRCS = main_emul.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
INCLUDES += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD)
CFLAGS += -g -O0 -D__EMUL__ -DPERF_COUNT=0 -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast
LFLAGS		=
LIBS			= -lm
BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = exe

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

all:: model $(MAIN)

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(MAIN)

include common/model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
