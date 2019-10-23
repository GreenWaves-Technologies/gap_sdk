# Copyright (c) 2017 GreenWaves Technologies SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of GreenWaves Technologies SAS nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

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
MBED_FLAGS     += -DW_HALFCHAR -DTILED -DDOMFCC $(USE_HARDWARE_CE) -DCHECK_WAV  -DVOCAL   -lm  -DALEXA  -DTESTNONE -DGAP_SOURCE_SCK0 #-DDUMP_SCORE -DPYTHON_SYNC #-DDUMP_SCORE
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
