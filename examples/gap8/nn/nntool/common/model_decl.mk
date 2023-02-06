# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

MODEL_SUFFIX?=

MODEL_PREFIX?=

MODEL_PYTHON=python3

MODEL_COMMON ?= common
MODEL_COMMON_INC ?= $(GAP_SDK_HOME)/libs/gap_lib/include
MODEL_COMMON_SRC ?= $(GAP_SDK_HOME)/libs/gap_lib/img_io
MODEL_COMMON_SRC_FILES ?= ImgIO.c
MODEL_COMMON_SRCS = $(realpath $(addprefix $(MODEL_COMMON_SRC)/,$(MODEL_COMMON_SRC_FILES)))
MODEL_BUILD = BUILD_MODEL$(MODEL_SUFFIX)

TENSORS_DIR = $(MODEL_BUILD)/tensors
ifndef MODEL_HAS_NO_CONSTANTS
  MODEL_TENSORS=$(MODEL_BUILD)/$(MODEL_PREFIX)_L3_Flash_Const.dat
else
  MODEL_TENSORS=
endif

MODEL_STATE = $(MODEL_BUILD)/$(MODEL_PREFIX).json
MODEL_PATH = $(MODEL_BUILD)/$(MODEL_PREFIX)$(suffix $(TRAINED_MODEL))
MODEL_SRC = $(MODEL_PREFIX)Model.c
AT_MODEL_PATH = $(MODEL_BUILD)/$(MODEL_SRC)

MODEL_HEADER = $(MODEL_PREFIX)Info.h
MODEL_GEN = $(MODEL_BUILD)/$(MODEL_PREFIX)Kernels 
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
MODEL_GEN_EXE = $(MODEL_BUILD)/GenTile

ifdef MODEL_QUANTIZED
  NNTOOL_EXTRA_FLAGS = -q
endif

MODEL_GENFLAGS_EXTRA?=
EXTRA_GENERATOR_SRC?=
NNTOOL_SCRIPT?=
MODEL_GEN_AT_LOGLEVEL?=1
MODEL_GEN_EXTRA_FLAGS += --log_level=$(MODEL_GEN_AT_LOGLEVEL)

$(info script $(NNTOOL_SCRIPT))
RM=rm -f

NNTOOL?=nntool

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
CLUSTER_SLAVE_PE=8
else ifeq '$(TARGET_CHIP_FAMILY)' 'GAP8'
CLUSTER_SLAVE_PE=7
else
  $(error TARGE_CHIP_FAMILY not found in env or not correct)
endif

# Options for the memory settings: will require
# set l3_flash_device $(MODEL_L3_FLASH)
# set l3_ram_device $(MODEL_L3_RAM)
# in the nntool_script
# FLASH and RAM type
FLASH_TYPE = DEFAULT
RAM_TYPE   = DEFAULT

ifeq '$(FLASH_TYPE)' 'HYPER'
    MODEL_L3_FLASH=AT_MEM_L3_HFLASH
else ifeq '$(FLASH_TYPE)' 'MRAM'
    MODEL_L3_FLASH=AT_MEM_L3_MRAMFLASH
    READFS_FLASH = target/chip/soc/mram
else ifeq '$(FLASH_TYPE)' 'QSPI'
    MODEL_L3_FLASH=AT_MEM_L3_QSPIFLASH
    READFS_FLASH = target/board/devices/spiflash
else ifeq '$(FLASH_TYPE)' 'OSPI'
    MODEL_L3_FLASH=AT_MEM_L3_OSPIFLASH
else ifeq '$(FLASH_TYPE)' 'DEFAULT'
    MODEL_L3_FLASH=AT_MEM_L3_DEFAULTFLASH
endif

ifeq '$(RAM_TYPE)' 'HYPER'
    MODEL_L3_RAM=AT_MEM_L3_HRAM
else ifeq '$(RAM_TYPE)' 'QSPI'
    MODEL_L3_RAM=AT_MEM_L3_QSPIRAM
else ifeq '$(RAM_TYPE)' 'OSPI'
    MODEL_L3_RAM=AT_MEM_L3_OSPIRAM
else ifeq '$(RAM_TYPE)' 'DEFAULT'
    MODEL_L3_RAM=AT_MEM_L3_DEFAULTRAM
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

# Memory sizes for cluster L1, SoC L2 and Flash
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
  TARGET_L1_SIZE ?= 128000
  TARGET_L2_SIZE ?= 1400000
  TARGET_L3_SIZE ?= 8000000
else
  TARGET_L1_SIZE ?= 64000
  TARGET_L2_SIZE ?= 400000
  TARGET_L3_SIZE ?= 8000000
endif

# Cluster stack size for master core and other cores
CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024


TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* $(CLUSTER_SLAVE_PE))
MODEL_L1_MEMORY=$(shell expr $(TARGET_L1_SIZE) \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=$(TARGET_L2_SIZE)
MODEL_L3_MEMORY=$(TARGET_L3_SIZE)


include $(RULES_DIR)/at_common_decl.mk
$(info GEN ... $(CNN_GEN))
