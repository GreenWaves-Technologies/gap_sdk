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

pulpChip = GAP
PULP_APP = meterOCR
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler_v2
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
MNIST_GEN_PATH = $(TILER_GENERATOR_PATH)/CNN/generator
MNIST_KER_PATH = $(TILER_GENERATOR_PATH)/CNN/kernels

PULP_APP_SRCS = Mnist.c MnistKernels.c MnistKernelsInit.c $(MNIST_KER_PATH)/src/CNN_BiasReLULinear_BasicKernels.c $(MNIST_KER_PATH)/src/CNN_Conv_BasicKernels.c $(MNIST_KER_PATH)/src/CNN_Pooling_BasicKernels.c ImgIO.c
PULP_APP_SRCS += $(MNIST_KER_PATH)/src/CNN_HwCE.c
pulpFc=1
RM=rm -f
NB_FRAMES ?= -1

# Should be removed once openocd is the default bridge
export GAP_USE_OPENOCD=1
io=host

#To activeate Hardware CE SET -DRT_HAS_HWCE=1
#USE_HARDWARE_CE = -DRT_HAS_HWCE=1

PULP_CFLAGS += -O3 $(USE_HARDWARE_CE)
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns  -fdata-sections -ffunction-sections
PULP_LDFLAGS +=  -flto -Wl,--gc-sections
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Wno-maybe-uninitialized -Wno-unused-but-set-variable
PULP_CFLAGS += -I$(TILER_INC) -I$(MNIST_KER_PATH)/include -DNB_FRAMES=$(NB_FRAMES)

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTileBuild:
	gcc -o GenMnist $(USE_HARDWARE_CE) -I$(TILER_INC) -I$(MNIST_GEN_PATH)/include MnistModel.c $(MNIST_GEN_PATH)/src/CNN_Generators.c $(TILER_LIB)

# Run the code generator
GenTile: GenTileBuild
	./GenMnist

model: GenTile

clean::
	$(RM) GenMnist MnistKernels.* MnistKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk

