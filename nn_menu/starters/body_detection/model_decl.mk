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

# Increase this to improve accuracy
MODEL_BUILD = BUILD_MODEL$(MODEL_SUFFIX)
MODEL_H5 = $(MODEL_TRAIN_BUILD)/$(MODEL_PREFIX).h5
MODEL_NAME = $(MODEL_PREFIX).tflite
MODEL_TFLITE = model/$(MODEL_NAME)

TENSORS_DIR = $(MODEL_BUILD)/tensors
MODEL_TENSORS = $(MODEL_BUILD)/$(MODEL_PREFIX)_L3_Flash_Const.dat

MODEL_STATE = $(MODEL_BUILD)/$(MODEL_PREFIX).json
MODEL_SRC = $(MODEL_PREFIX)Model.c
MODEL_GEN = $(MODEL_BUILD)/$(MODEL_PREFIX)Kernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
MODEL_GEN_EXE = ./GenTile
ifndef NNTOOL_SCRIPT
  NNTOOL_SCRIPT=model/nntool_script
endif
IMAGES = images
RM=rm -f

NNTOOL=nntool

MODEL_GENFLAGS_EXTRA= #-c "model/code_template.c"
MODEL_HEADER= $(MODEL_PREFIX)Info.h

MODEL_POW2=1
include $(RULES_DIR)/at_common_decl.mk
