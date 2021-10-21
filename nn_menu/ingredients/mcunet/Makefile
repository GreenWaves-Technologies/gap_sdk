# Copyright (C) 2021 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

MODEL_FORMAT?=tflite

include common.mk

io=host

MODEL_PREFIX=MCUNet

RAM_FLASH_TYPE ?= HYPER
#PMSIS_OS=freertos

ifeq '$(RAM_FLASH_TYPE)' 'HYPER'
APP_CFLAGS += -DUSE_HYPER
MODEL_L3_EXEC=hram
MODEL_L3_CONST=hflash
else
APP_CFLAGS += -DUSE_SPI
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

#pulpChip = GAP
#PULP_APP = $(MODEL_PREFIX)

CNN_KERNELS_SRC = \
  $(wildcard $(TILER_CNN_KERNEL_PATH_SQ8)/CNN_*SQ8.c) \
  $(TILER_CNN_KERNEL_PATH_SQ8)/CNN_AT_Misc.c \
  $(TILER_CNN_KERNEL_PATH)/CNN_CopyBasicKernels.c


APP_SRCS = \
  main.c \
  $(MODEL_BUILD)/$(MODEL_PREFIX)Kernels.c \
  $(CNN_KERNELS_SRC)\
  $(GAP_LIB_PATH)/img_io/ImgIO.c

APP_INC += "$(PWD)" \
	"$(MODEL_BUILD)" \
	"$(TILER_INC)" \
	"$(TILER_CNN_KERNEL_PATH_FP16)" \
  "$(TILER_CNN_KERNEL_PATH_SQ8)" \
	"$(TILER_CNN_KERNEL_PATH)" \
	"$(GAP_LIB_PATH)/include" \
    

DATA_FILES = $(MODEL_BUILD)/$(MODEL_PREFIX)_L3_Flash_Const.dat
APP_CFLAGS += -O3 -g -DPERF_COUNT=0


READFS_FILES+=$(realpath $(DATA_FILES))

# all depends on the model
all:: model

clean:: clean_model

include common/model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
include $(RULES_DIR)/pmsis_rules.mk
