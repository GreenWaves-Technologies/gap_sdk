### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

MODEL_PREFIX=lynred
ifndef NN_BITS
  NN_BITS=16
endif

# LINK_IMAGE=samples/5223_5.pgm

$(info Building emulation mode with $(NN_BITS) bit quantization)

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifeq ($(NN_BITS),8)
  CFLAGS += -DNN_8BIT
  NNTOOL_SCRIPT=model/nntool_script8
  MODEL_SUFFIX = _EMUL8BIT
else
  ifeq ($(NN_BITS),16)
    CFLAGS += -DNN_16BIT
    NNTOOL_SCRIPT=model/nntool_script16
    MODEL_SUFFIX = _EMUL16BIT
  else
    $(error Dont know how to build with this bit width)
  endif
endif

include model_decl.mk

ifdef LINK_IMAGE
  LINK_IMAGE_HEADER=$(MODEL_BUILD)/image.h
  LINK_IMAGE_NAME=$(subst .,_,$(subst /,_,$(LINK_IMAGE)))
  GAP_FLAGS += -DLINK_IMAGE_HEADER="\"$(LINK_IMAGE_HEADER)\"" -DLINK_IMAGE_NAME="$(LINK_IMAGE_NAME)"
else
  LINK_IMAGE_HEADER=
endif

MODEL_GEN_EXTRA_FLAGS= -f $(MODEL_BUILD)
CC = gcc
CFLAGS += -g -O0 -D__EMUL__ 
INCLUDES = -I. -Ii./helpers -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(GEN_PATH) -I$(MODEL_BUILD)
LFLAGS = -lm
LIBS =
SRCS += Mnist.c ImgIO.c Seq_SSD.c helpers/helpers.c $(MODEL_SRCS)


BUILD_DIR = BUILD_EMUL

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

MAIN = mnist_emul

# Here we set the memory allocation for the generated kernels
# REMEMBER THAT THE L1 MEMORY ALLOCATION MUST INCLUDE SPACE
# FOR ALLOCATED STACKS!
MODEL_L1_MEMORY=40000
MODEL_L2_MEMORY=100000
MODEL_L3_MEMORY=8388608
# hram - HyperBus RAM
# qspiram - Quad SPI RAM
MODEL_L3_EXEC=hram
# hflash - HyperBus Flash
# qpsiflash - Quad SPI Flash
MODEL_L3_CONST=hflash

all: model $(MAIN)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

ifdef LINK_IMAGE
$(LINK_IMAGE_HEADER): $(LINK_IMAGE)
	xxd -i $< $@
endif

clean: clean_model
	$(RM) -r $(BUILD_DIR)
	$(RM) $(MAIN)

.PHONY: depend clean

include model_rules.mk
