# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

include common.mk
include $(RULES_DIR)/at_common_decl.mk

io?=host

FLASH_TYPE ?= HYPER
RAM_TYPE   ?= HYPER
#PMSIS_OS=freertos

ifeq '$(FLASH_TYPE)' 'HYPER'
  MODEL_L3_CONST=AT_MEM_L3_HFLASH
else ifeq '$(FLASH_TYPE)' 'MRAM'
  MODEL_L3_CONST=AT_MEM_L3_MRAMFLASH
  READFS_FLASH = target/chip/soc/mram
else ifeq '$(FLASH_TYPE)' 'QSPI'
  MODEL_L3_CONST=AT_MEM_L3_QSPIFLASH
  READFS_FLASH = target/board/devices/spiflash
else ifeq '$(FLASH_TYPE)' 'OSPI'
  MODEL_L3_CONST=AT_MEM_L3_OSPIFLASH
  READFS_FLASH = target/board/devices/ospiflash
endif

ifeq '$(RAM_TYPE)' 'HYPER'
  MODEL_L3_EXEC=AT_MEM_L3_HRAM
else ifeq '$(RAM_TYPE)' 'QSPI'
  MODEL_L3_EXEC=AT_MEM_L3_QSPIRAM
else ifeq '$(RAM_TYPE)' 'OSPI'
  MODEL_L3_EXEC=AT_MEM_L3_OSPIRAM
endif

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
FREQ_CL?=370
FREQ_FC?=370
FREQ_PE?=370
else
ifeq '$(TARGET_CHIP)' 'GAP8_V3'
FREQ_CL?=175
else
FREQ_CL?=50
endif
FREQ_FC?=250
FREQ_PE?=250
endif

$(info Building NNTOOL model)
NNTOOL_EXTRA_FLAGS ?= 

include common/model_decl.mk

# pulpChip = GAP
# PULP_APP = $(MODEL_PREFIX)

APP = $(MODEL_PREFIX)
APP_SRCS += $(MODEL_PREFIX).c $(MODEL_GEN_C) $(MODEL_EXPRESSIONS) $(MODEL_COMMON_SRCS) $(CNN_LIB)

APP_CFLAGS += -g -O3 -mno-memcpy -fno-tree-loop-distribute-patterns
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD)
APP_CFLAGS += -DPERF -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE) -DFREQ_FC=$(FREQ_FC) -DFREQ_CL=$(FREQ_CL) -DFREQ_PE=$(FREQ_PE)

READFS_FILES=$(abspath $(MODEL_TENSORS))

# all depends on the model
all:: model

clean:: clean_model

include common/model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
include $(RULES_DIR)/pmsis_rules.mk

