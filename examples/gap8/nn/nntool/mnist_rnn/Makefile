# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

MODEL_PREFIX=mnist
AT_INPUT_WIDTH=28
AT_INPUT_HEIGHT=28
AT_INPUT_COLORS=1
IMAGES = images

LOAD_QUANTIZATION= #-q #to load a tflite quantized model
IMAGE=$(CURDIR)/samples/5223_5.pgm

io=host

QUANT_BITS?=8
BUILD_DIR=BUILD
MODEL_SQ8=1
NNTOOL_SCRIPT=model/nntool_script
MODEL_SUFFIX = _SQ8BIT

$(info Building GAP8 mode with $(QUANT_BITS) bit quantization)

TRAINED_MODEL=model/mnist.tflite
include ../common/model_decl.mk

APP = mnist
APP_SRCS += $(MODEL_PREFIX).c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB)

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP8'
        APP_CFLAGS += -march=rv32imcxgap8
endif
APP_CFLAGS += -g -O3 -mno-memcpy -fno-tree-loop-distribute-patterns
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD)
APP_CFLAGS += -DPERF -DAT_MODEL_PREFIX=$(MODEL_PREFIX) $(MODEL_SIZE_CFLAGS)
APP_CFLAGS += -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DAT_IMAGE=$(IMAGE)

READFS_FILES=$(abspath $(MODEL_TENSORS))

# all depends on the model
all:: model

clean:: clean_model

$(IMAGES):
	echo "GENERATING INPUT IMAGES"
	(mkdir -p $(IMAGES); $(MODEL_PYTHON) model/save_samples.py -d $@ -n 5)

include ../common/model_rules.mk
$(info APP_SRCS... $(APP_SRCS))
$(info APP_CFLAGS... $(APP_CFLAGS))
include $(RULES_DIR)/pmsis_rules.mk

