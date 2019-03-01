# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = Mnist
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
MNIST_GEN_PATH = $(TILER_GENERATOR_PATH)/CNN/generator
MNIST_KER_PATH = $(TILER_GENERATOR_PATH)/CNN/kernels

PULP_APP_SRCS = Mnist.c MnistKernels.c MnistKernelsInit.c $(MNIST_KER_PATH)/src/CNN_BiasReLULinear_BasicKernels.c $(MNIST_KER_PATH)/src/CNN_Conv_BasicKernels.c $(MNIST_KER_PATH)/src/CNN_Pooling_BasicKernels.c ImgIO.c
PULP_APP_SRCS += $(MNIST_KER_PATH)/src/CNN_HwCE.c
pulpFc=1
RM=rm -f

#To activeate Hardware CE SET -DRT_HAS_HWCE=1
USE_HARDWARE_CE = -DRT_HAS_HWCE=0

PULP_CFLAGS += -O3 $(USE_HARDWARE_CE)
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns  -fdata-sections -ffunction-sections
PULP_LDFLAGS +=  -flto -Wl,--gc-sections
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Wno-maybe-uninitialized -Wno-unused-but-set-variable
PULP_CFLAGS += -I$(TILER_INC) -I$(MNIST_KER_PATH)/include

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

