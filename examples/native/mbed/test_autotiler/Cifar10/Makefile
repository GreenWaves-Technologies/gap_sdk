# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
CIFAR_GEN_PATH = $(TILER_GENERATOR_PATH)/CNN/generator
CIFAR_KER_PATH = $(TILER_GENERATOR_PATH)/CNN/kernels
TEST_C = Cifar10.c Cifar10Kernels.c Cifar10KernelsInit.c $(CIFAR_KER_PATH)/src/CNN_BiasReLULinear_BasicKernels.c $(CIFAR_KER_PATH)/src/CNN_Conv_BasicKernels.c $(CIFAR_KER_PATH)/src/CNN_Pooling_BasicKernels.c
TEST_C += $(CIFAR_KER_PATH)/src/CNN_HwCE.c

#To activeate Hardware CE SET -DRT_HAS_HWCE=1
USE_HARDWARE_CE = -DRT_HAS_HWCE=0

MBED_FLAGS += -O3 $(USE_HARDWARE_CE)

RM=rm -f

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
INC_PATH += -I$(TILER_INC) -I$(CIFAR_KER_PATH)/include

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenCifar10 $(USE_HARDWARE_CE) -I$(TILER_INC) -I$(CIFAR_GEN_PATH)/include Cifar10Model.c $(CIFAR_GEN_PATH)/src/CNN_Generators.c $(TILER_LIB)

# Run the code generator
Cifar10Kernels.c: GenTile
	./GenCifar10 -m

model: Cifar10Kernels.c

clean::
	$(RM) GenCifar10 Cifar10Kernels.* Cifar10KernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

