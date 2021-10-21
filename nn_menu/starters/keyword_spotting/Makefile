# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

#To Use real mic in sensorboard set this variable to 1
USE_MIC_SENSORBOARD?=0
USE_EXT_CLK?=0


WITH_MFCC ?= 1
USE_POWER ?= 1
USE_HIGH_PREC ?= 0
NN_TYPE  ?= SMALL
DCT_COUNT = 10
FRAME_SIZE_ms = 40
FRAME_STEP_ms = 20
FRAME_SIZE = 640
FRAME_STEP = 320
AT_INPUT_WIDTH=10
AT_INPUT_HEIGHT=49

ifeq ($(NN_TYPE), SMALL)
	MODEL_PREFIX = KWS_ds_cnn_s_quant
	APP_CFLAGS += -DSMALL
else
	ifeq ($(NN_TYPE), MEDIUM)
		MODEL_PREFIX = KWS_ds_cnn_m_quant
		APP_CFLAGS += -DMEDIUM
	else
		ifeq ($(NN_TYPE), LARGE)
			MODEL_PREFIX = KWS_ds_cnn_l_quant
			APP_CFLAGS += -DLARGE
		endif
	endif
endif
MODEL_SQ8=1
pulpChip = GAP
RM=rm -f

IMAGE = $(CURDIR)/samples/on_sample_features_int8.pgm
WAV_PATH = $(CURDIR)/samples/on_sample.wav

READFS_FILES=$(realpath $(MODEL_TENSORS))

QUANT_BITS=8
BUILD_DIR=BUILD

#MODEL_NE16 ?= 0
NNTOOL_SCRIPT=model/nntool_script
MODEL_SUFFIX = _SQ$(QUANT_BITS)
ifeq ($(MODEL_NE16), 1)
	NNTOOL_SCRIPT=model/nntool_script_ne16
	MODEL_SUFFIX = _NE16
endif
NNTOOL_EXTRA_FLAGS = -q
MODEL_BUILD = BUILD_MODEL$(MODEL_SUFFIX)

CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024
TOTAL_STACK_SIZE=$(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
MODEL_L2_MEMORY=350000
MODEL_L3_MEMORY=8000000
MODEL_SIZE_CFLAGS = -DAT_INPUT_HEIGHT=$(AT_INPUT_HEIGHT) -DAT_INPUT_WIDTH=$(AT_INPUT_WIDTH) -DAT_INPUT_COLORS=$(AT_INPUT_COLORS)
ifeq '$(TARGET_CHIP)' 'GAP8_V3'
	FREQ_CL?=175
else
	FREQ_CL?=50
endif
FREQ_FC?=250
VOLTAGE?=1.2

ifeq ($(USE_POWER), 1)
	# override the tflite model name to the one which expects power MFCC -> more efficient
	TRAINED_MODEL=model/$(MODEL_PREFIX)_power.tflite
else
	TRAINED_MODEL=model/$(MODEL_PREFIX).tflite
endif
include common/model_decl.mk
include mfcc_model.mk


ifeq ($(WITH_MFCC), 1)
	APP_SRCS    += main_with_mfcc.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
	APP_SRCS    += $(GAP_LIB_PATH)/wav_io/wavIO.c $(MFCCBUILD_DIR)/MFCCKernels.c
	APP_CFLAGS  += -I$(MFCC_GENERATOR) -I$(MFCCBUILD_DIR)
else
	APP_SRCS    += main.c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)
endif
ifeq ($(PMSIS_OS), freertos)
	APP_CFLAGS += -DFREERTOS
endif

APP_CFLAGS += -O3 -s -mno-memcpy -fno-tree-loop-distribute-patterns 
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(MODEL_BUILD) $(CNN_LIB_INCLUDE)
APP_CFLAGS += -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE) -DFREQ_FC=$(FREQ_FC) -DFREQ_CL=$(FREQ_CL) -DVOLTAGE=$(VOLTAGE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE) -DAT_WAV=$(WAV_PATH)

ifeq ($(USE_MIC_SENSORBOARD), 1)
	APP_CFLAGS += -DFROM_SENSOR -DSILENT
	ifeq ($(USE_EXT_CLK), 1)
		APP_CFLAGS += -DUSE_EXT_CLK
	endif
else
	APP_CFLAGS += -DPERF
endif
LIBS = -lm

generate_samples:
	python3 utils/generate_samples_images.py --dct_coefficient_count $(DCT_COUNT) --window_size_ms $(FRAME_SIZE_ms) --window_stride_ms $(FRAME_STEP_ms)

test_accuracy:
	python3 utils/test_accuracy_emul.py --tflite_model $(TRAINED_MODEL) --dct_coefficient_count $(DCT_COUNT) --window_size_ms $(FRAME_SIZE_ms) --window_stride_ms $(FRAME_STEP_ms) --test_with_wav $(WITH_MFCC) --use_power_spectrogram $(USE_POWER)

test_accuracy_tflite:
	python3 utils/test_accuracy_tflite.py --tflite_model $(TRAINED_MODEL) --dct_coefficient_count $(DCT_COUNT) --window_size_ms $(FRAME_SIZE_ms) --window_stride_ms $(FRAME_STEP_ms) --use_power_spectrogram $(USE_POWER)

# all depends on the model
all::  $(MFCCBUILD_DIR)/MFCCKernels.c model

clean:: clean_model clean_mfcc_code

at_model_disp:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS) --debug=Disp

include common/model_rules.mk
include $(RULES_DIR)/pmsis_rules.mk
