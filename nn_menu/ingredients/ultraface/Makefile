# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

include common.mk
include $(RULES_DIR)/at_common_decl.mk

io=host

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
USE_PMSIS_BSP=1
export GAP_USE_OPENOCD=1

CNN_KERNELS_SRC = \
  $(wildcard $(TILER_CNN_KERNEL_PATH_SQ8)/CNN_*SQ8.c) \
  $(TILER_CNN_KERNEL_PATH_SQ8)/CNN_AT_Misc.c \
  $(TILER_CNN_KERNEL_PATH)/CNN_CopyBasicKernels.c \
  $(TILER_DSP_KERNEL_PATH)/math_funcs.c

APP_SRCS = \
  gap_inference.c \
  nms.c \
  $(MODEL_BUILD)/modelKernels.c \
  $(MODEL_BUILD)/Expression_Kernels.c \
  $(CNN_KERNELS_SRC)\
  $(GAP_LIB_PATH)/img_io/ImgIO.c \

APP_INC += "$(MODEL_BUILD)" \
	"$(TILER_INC)" \
	"$(TILER_CNN_KERNEL_PATH_SQ8)" \
	"$(TILER_CNN_KERNEL_PATH_FP16)" \
	"$(TILER_CNN_KERNEL_PATH)" \
	"$(TILER_DSP_KERNEL_PATH)" \
	"$(TILER_CNN_GENERATOR_PATH)" \
	"$(TILER_CNN_GENERATOR_PATH_SQ8)" \
	"$(GAP_LIB_PATH)/include" \

DATA_FILES = model_L3_Flash_Const.dat
APP_CFLAGS += -g -O3
LIBS = -lm

ifeq ($(ALREADY_FLASHED),1)
# everything is already on board
else
  READFS_FILES+=$(realpath $(DATA_FILES))
endif

# all depends on the model
all:: model

clean:: clean_model

include common/model_rules.mk
$(info APP_INC... $(APP_INC))
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
include $(RULES_DIR)/pmsis_rules.mk
