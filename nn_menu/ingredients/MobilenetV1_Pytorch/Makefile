# Copyright 2019 GreenWaves Technologies, SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


APP = Mobilenet

PMSIS_OS=pulpos

include $(RULES_DIR)/at_common_decl.mk

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
	FREQ_CL?=50
	FREQ_FC?=50
	MODEL_L1_MEMORY=110000
	MODEL_L2_MEMORY=1300000
	MODEL_L3_MEMORY=8388608
else
	ifeq '$(TARGET_CHIP)' 'GAP8_V3'
		FREQ_CL?=175
	else
		FREQ_CL?=50
	endif
	FREQ_FC?=250
	MODEL_L1_MEMORY=50000
	MODEL_L2_MEMORY?=350000
	MODEL_L3_MEMORY=8000000
endif

ifdef MODEL_L1_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L1 $(MODEL_L1_MEMORY)
endif

ifdef MODEL_L2_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L2 $(MODEL_L2_MEMORY)
endif

ifdef MODEL_L3_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L3 $(MODEL_L3_MEMORY)
endif

MOBILENET_GEN_PATH = $(TILER_CNN_GENERATOR_PATH)

MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_BiasReLULinear_BasicKernels.c
MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_Pooling_BasicKernels.c
MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_BasicKernels.c
MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_DP_BasicKernels.c
MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_SoftMax.c
MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_DW_BasicKernels.c
MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_MatAlgebra.c
MOBILENET_KER_PATH += $(TILER_CNN_KERNEL_PATH)/CNN_Conv_DW_DP_BasicKernels.c

TILER_APP_SRCS = MobileNetModel.c $(MOBILENET_GEN_PATH)/CNN_Generators.c $(MOBILENET_GEN_PATH)/CNN_Generator_Util.c $(TILER_CNN_GENERATOR_PATH)/CNN_Copy_Generators.c
TILER_EXE = GenMobilenet

TILER_USER_KERNELS = MN_Kernels.c 

APP_SRCS = main.c $(TILER_USER_KERNELS) $(MOBILENET_KER_PATH)
APP_SRCS += utils/src/dnn_utils.c
APP_INC += $(TILER_INC) $(CNN_AT_PATH)
RM=rm -f


APP_CFLAGS += -O3 

APP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns  -fdata-sections -ffunction-sections 
APP_LDFLAGS += -flto -Wl,--gc-sections 

APP_CFLAGS += -w -Wno-maybe-uninitialized -Wno-unused-but-set-variable
APP_CFLAGS += -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(MOBILENET_GEN_PATH) -Iutils/inc -I$(TILER_CNN_KERNEL_PATH)
APP_CFLAGS += $(CNN_LIB_INCLUDE)

io=host

ifeq ($(ALREADY_FLASHED),)
	READFS_FILES = $(realpath MN_L3_Flash_Const.bin) $(realpath binFiles/L0_INPUT.bin)
	PLPBRIDGE_FLAGS += -f 
endif


# Build the code generator
$(TILER_EXE): 
	gcc -o GenMobilenet -Imbnets -I$(CNN_GEN_INCLUDE) -I$(MOBILENET_GEN_PATH) -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(MOBILENET_GEN_PATH)/include $(TILER_APP_SRCS) $(TILER_LIB)

# Run the code generator and generated user kernels
$(TILER_USER_KERNELS): $(TILER_EXE)
	./GenMobilenet $(MODEL_GEN_EXTRA_FLAGS)

model: $(TILER_USER_KERNELS)

all:: model

clean::
	$(RM) GenMobilenet MN_Kernels.* MobilenetKernels.* GenMobilenetGraph

.PHONY: model clean

include $(RULES_DIR)/pmsis_rules.mk
