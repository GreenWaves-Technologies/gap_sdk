# Copyright 2021 GreenWaves Technologies, SAS
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

include $(CUSTOM_BSP_PATH)/bsp_src.mk

GEN_PATH = $(TILER_GENERATOR_PATH)/DeMosaic
KER_PATH = $(TILER_GENERATOR_PATH)/DeMosaic

APP              = test
APP_SRCS        += test.c $(GAP_LIB_PATH)/img_io/ImgIO.c 
#APP_SRCS        += $(KER_PATH)/DeMosaicBasicKernels.c DeMosaicKernels.c
APP_SRCS        += DeMosaicBasicKernels.c DeMosaicKernels.c
APP_SRCS        += $(GAPOC_C_SRC)
APP_INC         += . $(GAP_LIB_PATH)/include $(TILER_INC) #$(TILER_GENERATOR_PATH)/DeMosaic
APP_INC         += $(CUSTOM_BSP_PATH)/bsp $(PMSIS_BSP_PATH)/include
APP_CFLAGS      += $(GAPOC_C_BSP_FLAGS)
APP_CFLAGS      += -O3 -g -DFRAME_DROP -DIMG_QVGA -DRUN_ENCODER_ON_FC

APP_CFLAGS      += 	-DSTACK_SIZE=4096
APP_CFLAGS      += 	-DSLAVE_STACK_SIZE=1024 

#Jpeg Quality Choises are:
#-DJPEG_Q_0
#-DJPEG_Q_10
#-DJPEG_Q_20
#-DJPEG_Q_50 //Default value
#-DJPEG_Q_90
#-DJPEG_Q_95
#-DJPEG_Q_100
APP_CFLAGS      += 	-DJPEG_Q_20

CONFIG_GAP_LIB_JPEG = 1
io=host
CUSTOM_BSP = 1


MODEL_SRC = demosaicModel.c
MODEL_GEN = DeMosaicKernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))


CUSTOM_BSP = 1

all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(GEN_PATH) $(MODEL_SRC) $(GEN_PATH)/DeMosaicGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)
	$(RM) resized_out.ppm

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk
