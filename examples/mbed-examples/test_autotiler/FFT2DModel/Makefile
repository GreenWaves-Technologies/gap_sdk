# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

PULP_APP = TestFFT2D
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_LIB = $(TILER_PATH)/lib/libtile.a
TILER_INC = $(TILER_PATH)/include
TILER_GENERATOR_PATH = $(TILER_PATH)/generators
FFT2D_GEN_PATH = $(TILER_GENERATOR_PATH)/FFT2D/generator
FFT2D_KER_PATH = $(TILER_GENERATOR_PATH)/FFT2D/kernels
FFT2D_TWIDDLES_PATH = $(TILER_GENERATOR_PATH)/FFT2D/twiddles_gen


TEST_C = TestFFT2D.c $(FFT2D_KER_PATH)/src/FFTBasicKernels.c $(FFT2D_KER_PATH)/src/FFTScalarBasicKernels.c TwiddlesDef.c SwapTablesDef.c FFT2DKernels.c FFT2DKernelsInit.c
RM=rm -f

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
MBED_FLAGS += -DFFTR2_128 -DFFTR4_256
INC_PATH += -I$(TILER_INC) -I$(FFT2D_KER_PATH)/include

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model twiddles

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(FFT2D_GEN_PATH)/include FFT2DModel.c $(FFT2D_GEN_PATH)/src/FFT2DGenerator.c $(TILER_LIB)

# Run the code generator
FFT2D.c FFT2D_Init.c: GenTile
	./GenTile -m

model: FFT2D.c FFT2D_Init.c

gen_twiddles:
	gcc -o GenTwiddles $(FFT2D_TWIDDLES_PATH)/GenTwid_Swap.c -lm

SwapTablesDef.c TwiddlesDef.c: gen_twiddles
	./GenTwiddles

twiddles: SwapTablesDef.c TwiddlesDef.c

clean::
	$(RM) GenTile GenTwiddles FFT2DKernels.*  FFT2DKernelsInit.* SwapTablesDef.* TwiddlesDef.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

