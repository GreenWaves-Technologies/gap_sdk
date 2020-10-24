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

# User Test
#------------------------------------------
ifndef GAP_SDK_HOME
	$(error Source sourceme in gap_sdk first)
endif

MODEL_PREFIX=kws

ifndef KWS_BITS
	KWS_BITS=16
endif

$(info Building GAP8 mode with $(KWS_BITS) bit quantization)

$(info Configure 16 bit model)
APP_CFLAGS += -DKWS_16BIT
NNTOOL_SCRIPT=model/nntool_script16
MODEL_SUFFIX = _16BIT

include model_decl.mk


# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
MODEL_L1_MEMORY=52000
MODEL_L2_MEMORY=307200
MODEL_L3_MEMORY=8388608
# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

pulpChip = GAP

# use a custom template to switch on the performance checking
MODEL_GENFLAGS_EXTRA= -c "model/code_template.c"

KWS_PATH = $(CURDIR)/KWS

APP_SRCS   += ./main.c ./wav_out.c $(KWS_PATH)/MFCC/MFCC_Processing.c $(KWS_PATH)/MFCC/MFCC.c
APP_SRCS   +=  $(MODEL_SRCS) $(KWS_PATH)/MFCC/MFCC_Dump.c

#		$(KWS_PATH)/CNN_KERNELS/CnnKernels.c  $(KWS_PATH)/CNN_KERNELS/CnnKernels_tileddense.c \
	#		$(KWS_PATH)/CNN_KERNELS/CNN_BasicKernels.c $(KWS_PATH)/CNN_KERNELS/CNN_HwCE.c  \

APP_INC            += $(KWS_PATH)/MFCC  $(KWS_PATH)/WEIGHTS_BIAS $(KWS_PATH)/CNN_KERNELS $(KWS_PATH)/include
APP_INC         += . ./helpers $(TILER_EMU_INC) $(TILER_INC) $(GEN_PATH) $(MODEL_BUILD)

APP_CFLAGS += -O3 -s -mno-memcpy -fno-tree-loop-distribute-patterns -Wno-sign-compare -Wno-maybe-uninitialized -Wno-type-limits

# -DKWS for KWS, -DALEXA for ALEXA
APP_CFLAGS     += -DW_HALFCHAR -DTILED -DDOMFCC $(USE_HARDWARE_CE) -DCHECK_WAV  -DVOCAL   -lm  -DKWS   -DGAP_SOURCE_SCK0 #-DFORCE_INPUT -DDUMP_SCORE -DPYTHON_SYNC #-DDUMP_SCORE



READFS_FILES = $(realpath $(MODEL_TENSORS))



APP_CFLAGS += -DNO_IMAGE

# all depends on the model
all:: model


#APP_LDFLAGS  += vocal_timelimit.lib

clean:: clean_model
clean_all: rm -rf BUILD

include model_rules.mk
include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
