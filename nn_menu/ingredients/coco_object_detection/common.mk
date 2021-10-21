NNTOOL=nntool
MODEL_SQ8=1

MODEL_SUFFIX?=
MODEL_PREFIX?=ssd_model
MODEL_PYTHON=python3
MODEL_BUILD=BUILD_MODEL$(MODEL_SUFFIX)

TRAINED_MODEL ?= models/ssd_mobv1_quant.tflite

MODEL_EXPRESSIONS = 

MODEL_QUANTIZED=1

# Memory sizes for cluster L1, SoC L2 and Flash
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
	TARGET_L1_SIZE = 128000
	TARGET_L2_SIZE = 1300000
	TARGET_L3_SIZE = 8000000
	FREQ_CL?=400
	FREQ_FC?=400
else
	TARGET_L1_SIZE = 64000
	TARGET_L2_SIZE = 300000
	TARGET_L3_SIZE = 8000000
	ifeq '$(TARGET_CHIP)' 'GAP8_V3'
		FREQ_CL?=175
	else
		FREQ_CL?=50
	endif
	FREQ_FC?=250
endif

# Cluster stack size for master core and other cores
CLUSTER_STACK_SIZE=6096
CLUSTER_SLAVE_STACK_SIZE=1024
CLUSTER_NUM_CORES=8

NNTOOL_SCRIPT = models/nntool_script
ifeq ($(MODEL_NE16), 1)
	NNTOOL_SCRIPT=models/nntool_script_ne16	
	MODEL_SUFFIX = _NE16
	APP_CFLAGS += -Wno-discarded-qualifiers
	APP_CFLAGS += -DMODEL_NE16
	CFLAGS += -DMODEL_NE16
else
ifeq ($(MODEL_HWC), 1)
	NNTOOL_SCRIPT=models/nntool_script_hwc
	APP_CFLAGS += -DMODEL_HWC
	CFLAGS += -DMODEL_HWC
endif
endif


$(info GEN ... $(CNN_GEN))
