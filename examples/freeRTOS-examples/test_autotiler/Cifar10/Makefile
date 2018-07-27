# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_GENERATOR_PATH=$(TILER_PATH)/autotiler_generator
APP_SRC += Cifar10.c
DEMO_SRC += Cifar10Kernels.c Cifar10KernelsInit.c $(TILER_GENERATOR_PATH)/CnnStdModel/CNN_BasicKernels.c $(TILER_GENERATOR_PATH)/CnnStdModel/CNN_HwCE.c

#To activeate Hardware CE SET -DRT_HAS_HWCE=1
USE_HARDWARE_CE = -DRT_HAS_HWCE=0

FREERTOS_FLAGS += -O3 -D__MBED__ $(USE_HARDWARE_CE)

RM=rm -f

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
INC_PATH += -I$(TILER_PATH)/StdTypes -I$(TILER_GENERATOR_PATH)/CnnStdModel

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenCifar10 $(USE_HARDWARE_CE) -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes/ -I$(TILER_GENERATOR_PATH)/CnnStdModel Cifar10Generator.c $(TILER_GENERATOR_PATH)/CnnStdModel/CNN_Generator.c $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
Cifar10Kernels.c Cifar10KernelsInit.c: GenTile
	./GenCifar10 -m

model: Cifar10Kernels.c Cifar10KernelsInit.c

clean::
	$(RM) GenCifar10 Cifar10Kernels.* Cifar10KernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/freeRTOS_rules.mk

