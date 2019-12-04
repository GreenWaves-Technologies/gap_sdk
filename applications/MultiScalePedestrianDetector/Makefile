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

PULP_APP = HoG

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler_v2
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators

MODEL_SRC = HoGEstimGenerator.c $(TILER_GENERATOR_PATH)/Resize/generator/src/ResizeGenerator.c HoGEstimModel.c

MODEL_GEN = HoGEstimKernelsInit HoGEstimKernels
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))

RM=rm -f

# Should be removed once openocd is the default bridge
export GAP_USE_OPENOCD=1
io=host

PULP_APP_SRCS  = HoGEstim.c ImgIO.c pad.c ili9341.c ImageDraw.c HoGEstimBasicKernels.c HoGEstimManage.c
PULP_APP_SRCS += $(TILER_GENERATOR_PATH)/HoG/kernels/src/HoGBasicKernels.c $(TILER_GENERATOR_PATH)/Resize/kernels/src/ResizeBasicKernels.c $(MODEL_GEN_C)

PULP_CFLAGS += -O3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections -Wno-unused-but-set-variable
PULP_CFLAGS += -I$(TILER_INC) -I$(TILER_GENERATOR_PATH)/Resize/kernels/include -I$(TILER_GENERATOR_PATH)/HoG/kernels/include
PULP_LDFLAGS +=  -flto -Wl,--gc-sections

PLPBRIDGE_FLAGS += -fileIO 5
imgOut:
	mkdir $@

all:: model imgOut

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(TILER_GENERATOR_PATH)/HoG/kernels/include -I$(TILER_GENERATOR_PATH)/Resize/generator/include $(MODEL_SRC) $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)
	rm -rf imgOut

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
