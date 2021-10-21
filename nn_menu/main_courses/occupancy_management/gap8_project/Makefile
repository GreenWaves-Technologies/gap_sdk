### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.


#                                  #
# Application configuration flags: #
#                                  #

##  if you want to test the application with an input file from host PC use this flag:
##  Note that in this case the shutterless filtering is desactivated:
APP_CFLAGS += -DINPUT_FILE
## To test prefiltering with a raw image
#GAP_FLAGS += -DINPUT_RAW_FILE
##  To save output results to host PC
##  This is usefull for application debug
#SAVE_TO_PC=1
##  In full demo mode ble should be activated and silent as well, since printfs block the execution if set over jtag
##  To disable all printf (needed to flash application to boot from flash):
#APP_CFLAGS += -DSILENT=1
##  To enable Ble communication (with the demo android app):
#APP_CFLAGS += -DUSE_BLE
##  To take an image as reference offset at every boot you can use the following flags:
#APP_CFLAGS += -DOFFSET_IMAGE_EVERY_BOOT=1
##  To enable QSPI_FLASH/RAM instead of Hyper Flash/Ram:
##  QSPI or HYPER:
EXT_MEM_TYPE ?= HYPER
##  To enable Sleep feature (ONLY in PulpOS):
#APP_CFLAGS += -DSLEEP


#######################################

ifndef GAP_SDK_HOME
	$(error Source sourceme in gap_sdk first)
endif

MODEL_PREFIX=lynred
ifndef NN_BITS
	NN_BITS=16
endif

MAKEFILE_PATH := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

$(info Building GAP8 mode with $(NN_BITS) bit quantization)

# For debugging don't load an image
# Run the network with zeros
#NO_IMAGE=1

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(NN_BITS),8)
	APP_CFLAGS += -DNN_8BIT
	NNTOOL_SCRIPT=model/nntool_script8
	MODEL_SUFFIX = _8BIT
else
	ifeq ($(NN_BITS),16)
		APP_CFLAGS += -DNN_16BIT
		ifeq ($(EXT_MEM_TYPE),HYPER)
			NNTOOL_SCRIPT=model/nntool_script16
		else
			NNTOOL_SCRIPT=model/nntool_script16_spi
		endif
		MODEL_SUFFIX = _16BIT
	else
		$(error Don\'t know how to build with this bit width)
	endif
endif

include model_decl.mk

MAIN_STACK_SIZE=4096

# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
MODEL_L1_MEMORY=42000
MODEL_L2_MEMORY=200000
MODEL_L3_MEMORY=4000000

ifeq ($(EXT_MEM_TYPE),HYPER)
# hram - HyperBus RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
MODEL_L3_CONST=hflash
else
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=qspiram
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=qspiflash
endif

APP_CFLAGS += -D$(EXT_MEM_TYPE)

pulpChip = GAP
APP = PeopleDetection

APP_SRCS += main.c ImgIO.c SSDParams.c $(MODEL_GEN_C) SSDBasicKernels.c $(CNN_LIB) $(SSD_MODEL_GEN_C) ImageDraw.c BleNotifier.c

APP_CFLAGS += -w -O3 -s -mno-memcpy -fno-tree-loop-distribute-patterns
APP_CFLAGS += -I. -I./helpers -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(GEN_PATH) -I$(MODEL_BUILD) $(CNN_LIB_INCLUDE)
APP_LDFLAGS += -L$(MAKEFILE_PATH)/shutterless/ -lshutterless -lm
APP_INC +=

BOARD_NAME = gapoc_b
PMSIS_OS  ?= pulpos

#RUNNER_CONFIG = $(CURDIR)/config-$(TARGET_NAME).ini

ifeq ($(platform),gvsoc)
  $(info Platform is GVSOC)
  READFS_FILES=$(realpath $(MODEL_TENSORS)) $(realpath Calibration.bin)
  else
  READFS_FILES+=$(realpath $(MODEL_TENSORS)) $(realpath Calibration.bin)
  PLPBRIDGE_FLAGS+= -f
endif

io=host

ifeq ($(SAVE_TO_PC),1)
	APP_CFLAGS+=-DSAVE_TO_PC=1
prepare_debug:
	mkdir -p dump_out_imgs
	rm -rf dump_out_imgs/*
else
prepare_debug:
	true
endif

#####Here we add cutom kernels that are not yet integrated into AT libraries

SSD_MODEL_GEN = SSDKernels
SSD_MODEL_GEN_C = $(addsuffix .c, $(SSD_MODEL_GEN))
SSD_MODEL_GEN_CLEAN = $(SSD_MODEL_GEN_C) $(addsuffix .h, $(SSD_MODEL_GEN))
SSD_FLAGS = -D$(EXT_MEM_TYPE)

GenSSDTile: SSDModel.c
	gcc -g $(SSD_FLAGS) -o GenSSDTile -I"$(TILER_INC)" SSDModel.c $(TILER_LIB)

$(SSD_MODEL_GEN_C): GenSSDTile
	./GenSSDTile

SSD_model: $(SSD_MODEL_GEN_C)
	cd SSDParamsGenerator && $(MAKE) all run

# all depends on the model
all:: SSD_model model prepare_debug

clean_all: clean_model clean
	rm -rf GenSSDTile
	rm -rf BUILD*
	cd SSDParamsGenerator && $(MAKE) clean
	rm -rf SSDKernels.*

.PHONY: clean_all

include model_rules.mk
include $(RULES_DIR)/pmsis_rules.mk

