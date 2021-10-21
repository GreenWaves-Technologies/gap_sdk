NNTOOL=nntool
MODEL_SQ8=1



MODEL_SUFFIX?=
MODEL_PREFIX?=model
MODEL_PYTHON=python3
MODEL_BUILD=BUILD_MODEL$(MODEL_SUFFIX)
$(info  MODEL_FORMAT is $(MODEL_FORMAT))
ifeq ($(MODEL_FORMAT), onnx)
  TRAINED_MODEL = ../models/mcunet-512kb-2mb_imagenet.onnx
  MODEL_PATH = $(MODEL_BUILD)/$(MODEL_PREFIX).onnx
else ifeq ($(MODEL_FORMAT), tflite)
  TRAINED_MODEL = ../models/mcunet-512kb-2mb_imagenet.tflite
  MODEL_PATH = $(MODEL_BUILD)/$(MODEL_PREFIX).tflite
  MODEL_QUANTIZED=1
else
  $(error MODEL_FORMAT can only be tflite or onnx)
endif

TENSORS_DIR = $(MODEL_BUILD)/tensors
MODEL_TENSORS = $(MODEL_BUILD)/$(MODEL_PREFIX)_L3_Flash_Const.dat

MODEL_STATE = $(MODEL_BUILD)/$(MODEL_PREFIX).json
MODEL_SRC = $(MODEL_PREFIX)Model.c
MODEL_HEADER = $(MODEL_PREFIX)Info.h
MODEL_GEN = $(MODEL_BUILD)/$(MODEL_PREFIX)Kernels $(MODEL_BUILD)/Expression_Kernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
MODEL_GEN_EXE = $(MODEL_BUILD)/GenTile

MODEL_EXPRESSIONS = #$(MODEL_BUILD)/Expression_Kernels.c

# Memory sizes for cluster L1, SoC L2 and Flash
TARGET_L1_SIZE = 64000
TARGET_L2_SIZE = 300000
TARGET_L3_SIZE = 8000000

# Cluster stack size for master core and other cores
CLUSTER_STACK_SIZE=4096
CLUSTER_SLAVE_STACK_SIZE=1024
CLUSTER_NUM_CORES=8

ifeq ($(MODEL_FORMAT), onnx)
  NNTOOL_SCRIPT = ../models/nntool_scripts/nntool_script_onnx
  MODEL_QUANTIZED=0
else ifeq ($(MODEL_FORMAT), tflite)
  NNTOOL_SCRIPT = ../models/nntool_scripts/nntool_script_tflite
  MODEL_QUANTIZED=1
endif
$(info GEN ... $(CNN_GEN))
