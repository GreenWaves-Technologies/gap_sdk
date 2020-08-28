# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = 
FFT2D_GEN_PATH = $(TILER_FFT2D_GENERATOR_PATH)
FFT2D_KER_PATH = $(TILER_FFT2D_GENERATOR_PATH)
FFT2D_TWIDDLES_PATH = $(TILER_FFT2D_TWIDDLE_PATH)

APP_SRCS = TestFFT2D.c $(FFT2D_KER_PATH)/FFTBasicKernels.c $(FFT2D_KER_PATH)/FFTScalarBasicKernels.c TwiddlesDef.c SwapTablesDef.c FFT2DKernels.c 
RM=rm -f

APP_CFLAGS += -O3 -DFFTR2_128 -DFFTR4_256
APP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns  -fdata-sections -ffunction-sections
APP_LDFLAGS +=  -flto -Wl,--gc-sections
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
APP_CFLAGS += -w -Wno-maybe-uninitialized -Wno-unused-but-set-variable
APP_CFLAGS += -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(FFT2D_KER_PATH)

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model twiddles

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_INC) -I$(FFT2D_GEN_PATH) FFT2DModel.c $(FFT2D_GEN_PATH)/FFT2DGenerator.c $(TILER_LIB)

# Run the code generator
FFT2DKernels.c: GenTile
	./GenTile

model: FFT2DKernels.c


gen_twiddles:
	gcc -o GenTwiddles $(FFT2D_TWIDDLES_PATH)/GenTwid_Swap.c -lm

SwapTablesDef.c TwiddlesDef.c: gen_twiddles
	./GenTwiddles

twiddles: SwapTablesDef.c TwiddlesDef.c

clean::
	$(RM) GenTile GenTwiddles FFT2DKernels.* SwapTablesDef.* TwiddlesDef.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk

