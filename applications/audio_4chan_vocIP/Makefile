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
KWS_PATH = $(CURDIR)/KWS

TEST_C          = ./main.c $(KWS_PATH)/MFCC/MFCC_Processing.c $(KWS_PATH)/MFCC/MFCC.c \
		$(KWS_PATH)/CNN_KERNELS/CnnKernels.c  $(KWS_PATH)/CNN_KERNELS/CnnKernels_tileddense.c \
		$(KWS_PATH)/CNN_KERNELS/CNN_BasicKernels.c $(KWS_PATH)/CNN_KERNELS/CNN_HwCE.c  \

INC            += $(KWS_PATH)/MFCC  $(KWS_PATH)/WEIGHTS_BIAS $(KWS_PATH)/CNN_KERNELS $(KWS_PATH)/include

# If not use HWCE
#TEST_C         +=$(KWS_PATH)/CNN_KERNELS/CnnKernelsInit.c

# If use HWCE
USE_HARDWARE_CE =  -DRT_HAS_HWCE=1
TEST_C         += $(KWS_PATH)/CNN_KERNELS/CnnKernels_hwce.c $(KWS_PATH)/CNN_KERNELS/CnnKernelsInit_hwce.c

MBED_FLAGS +=  -Iinclude -Isrc  #-DDBG_BS
MBED_FLAGS +=  -Os  #-DDBG

#uncomment this flag for alexa
MBED_FLAGS     += -DW_HALFCHAR -DTILED -DDOMFCC $(USE_HARDWARE_CE) -DCHECK_WAV  -DVOCAL   -lm  -DKWS   -DGAP_SOURCE_SCK0 -DFORCE_INPUT #-DDUMP_SCORE -DPYTHON_SYNC #-DDUMP_SCORE
#uncomment this flag for KWS
#MBED_FLAGS     += -DW_HALFCHAR -DTILED -DDOMFCC $(USE_HARDWARE_CE) -DCHECK_WAV  -DVOCAL   -lm  -DKWS  -DGAP_SOURCE_SCK0 #-DDUMP_SCORE -DPYTHON_SYNC

#PULP_LDFLAGS += vocal.lib
#LIBS += -L./
LIBSFLAGS += vocal_timelimit.lib

# If you do not want to use RTOS, comment it.
# If you want use RTOS then uncomment it.
# MBED_FLAGS     += -DMBED_CONF_RTOS_PRESENT=1

# For RTS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

#uncomment if PYTHON sync is required (wav file generation)
#PLPBRIDGE_FLAGS += -s i2s@../../../i2s_vocal.py

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
