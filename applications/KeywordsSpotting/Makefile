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


TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler_v2
GEN_PATH=$(TILER_PATH)/autotiler_generator
MODEL_SRC = ./CNN_kernels/KwsGenerator.c
PULP_APP = main

PULP_APP_SRCS =   main.c  ./libs/MFCC.c ./libs/MFCC_Dump.c ./CnnKernels_hwce.c CNN_kernels/CnnKernels.c  ./CNN_kernels/CnnKernels_tileddense.c  ./CnnKernelsInit_hwce.c ./libs/CNN_BasicKernels.c ./libs/CNN_HwCE.c ./libs/Cluster_FLL_optimal_settings.c

#To activate Hardware CE SET -DRT_HAS_HWCE=1
USE_HARDWARE_CE = -DRT_HAS_HWCE=1

PULP_CFLAGS += -O3 $(USE_HARDWARE_CE)

PULP_CFLAGS  +=  -O3 -Wno-unused-but-set-variable  -Wno-overflow -Wno-sign-compare -DW_HALFCHAR -DTILED -DHWCE_ACTIVE  -DDOMFCC # -DDUMP_DENSE  -DDOMFCC -DPRINTINT -DPRINTFEAT #-DDUMP_LAYER #-DREF -DPRINTDEB_TILE
PULP_CFLAGS  += -I./include  -I./CNN_kernels -I./include_libs -I./weights_bias -lm  #-I$(GEN_PATH)/CnnStdModel

#override CONFIG_OPT += camera=himax system/camera/image-stream=$(CURDIR)/IMG.pgm

all:: model

# Build the code generator
GenTile:
	gcc $(USE_HARDWARE_CE)  -o GenTile -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes/ -I./include  $(MODEL_SRC) ./CNN_kernels/CNN_Generator.c $(TILER_PATH)/lib/libtile.a

# Run the code generator
CnnKernels_hwce.c CnnKernelsInit_hwce.c: GenTile
	./GenTile

cleanmodel: CnnKernels_hwce.c CnnKernelsInit_hwce.c

cleanmodel:
	$(RM) GenTile CnnKernels_hwce.* CnnKernelsInit_hwce.*


.PHONY: model

#include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
