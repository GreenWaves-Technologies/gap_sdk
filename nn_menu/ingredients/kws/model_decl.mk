# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

GEN_PATH = $(TILER_GENERATOR_PATH)/CNN

ifndef MODEL_SUFFIX
  MODEL_SUFFIX=
endif

ifndef MODEL_PREFIX
  MODEL_PREFIX=GapFlow
endif

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(KWS_BITS),8)
  TRAIN_SUFFIX=_8BIT
else
  ifeq ($(KWS_BITS),16)
    TRAIN_SUFFIX=_16BIT
  else
    $(error Dont know how to build with this bit width)
  endif
endif

MODEL_PYTHON=python3

# Increase this to improve accuracy
TRAINING_EPOCHS=10
MODEL_TRAIN = model/train.py
#MODEL_TRAIN = /home/joel/ARCHIVE_SDK/gap_sdk_old/tf2gap8/examples/kws/train.py
MODEL_FREEZE = model/freeze.py
MODEL_BUILD = BUILD_MODEL$(MODEL_SUFFIX)
MODEL_TRAIN_BUILD = BUILD_TRAIN$(MODEL_SUFFIX)
MODEL_TF = $(MODEL_TRAIN_BUILD)/conv.pbtxt
MODEL_TFLITE = model/$(MODEL_PREFIX).tflite
MODEL_NAME = $(MODEL_PREFIX).tflite
MODEL_HEADER = $(MODEL_PREFIX)Info.h

TENSORS_DIR = $(MODEL_BUILD)/tensors
MODEL_TENSORS = $(MODEL_BUILD)/$(MODEL_PREFIX)_L3_Flash_Const.dat

MODEL_STATE = $(MODEL_BUILD)/$(MODEL_PREFIX).json
MODEL_SRC = $(MODEL_PREFIX)Model.c
MODEL_GEN = $(MODEL_BUILD)/$(MODEL_PREFIX)Kernels 
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
MODEL_GEN_EXE = $(MODEL_BUILD)/GenTile

MODEL_GENFLAGS_EXTRA =

$(info script $(NNTOOL_SCRIPT))
ifndef NNTOOL_SCRIPT
  NNTOOL_SCRIPT=model/nntool_script
endif
IMAGES = images
RM=rm -f

NNTOOL=nntool

MODEL_SRCS += $(MODEL_GEN_C)
MODEL_POW2 = 1
include $(RULES_DIR)/at_common_decl.mk
