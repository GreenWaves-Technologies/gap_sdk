# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

MODEL_PREFIX = expressions
RM=rm -f

IMAGES = samples
INPUT_1=$(CURDIR)/samples/input_0.dat
INPUT_2=$(CURDIR)/samples/input_1.dat
INPUT_3=$(CURDIR)/samples/input_2.dat
INPUT_4=$(CURDIR)/samples/input_3.dat

io=host

MODEL_SQ8=1
QUANT_BITS=8
BUILD_DIR=BUILD

NNTOOL_SCRIPT_PARAMETRIC = model/nntool_script_params
NNTOOL_SCRIPT=model/nntool_script
TRAINED_MODEL=model/expressions.tflite
MODEL_SUFFIX = _$(QUANT_BITS)BIT

include ../common/model_decl.mk

READFS_FILES=$(realpath $(MODEL_TENSORS))

APP = expressions
APP_SRCS += $(MODEL_PREFIX).c $(MODEL_GEN_C) $(MODEL_COMMON_SRCS) $(CNN_LIB) $(MODEL_BUILD)/Expression_Kernels.c

APP_CFLAGS += -O3 -g -s -mno-memcpy -fno-tree-loop-distribute-patterns 
APP_CFLAGS += -I. -I$(MODEL_COMMON_INC) -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE) -I$(MODEL_BUILD)
APP_CFLAGS += -DPERF -DAT_MODEL_PREFIX=$(APP) -DSTACK_SIZE=$(CLUSTER_STACK_SIZE) -DSLAVE_STACK_SIZE=$(CLUSTER_SLAVE_STACK_SIZE)
APP_CFLAGS += -DINPUT_1=$(INPUT_1) -DINPUT_2=$(INPUT_2) -DINPUT_3=$(INPUT_3) -DINPUT_4=$(INPUT_4)

# all depends on the model
all:: model

clean:: clean_model

at_model_disp:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS) --debug=Disp

at_model:: $(MODEL_BUILD) $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)

include ../common/model_rules.mk

include $(RULES_DIR)/pmsis_rules.mk

