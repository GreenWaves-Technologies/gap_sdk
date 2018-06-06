# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

RM=rm -f
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_GENERATOR_PATH=$(TILER_PATH)/autotiler_generator
TEST_C = TestFFT2D.c $(TILER_GENERATOR_PATH)/FFT_Lib/FFT_Lib.c $(TILER_GENERATOR_PATH)/FFT_Lib/FFT_Scalar_Lib.c $(TILER_GENERATOR_PATH)/FFT_Lib/TwiddlesDef.c $(TILER_GENERATOR_PATH)/FFT_Lib/SwapTablesDef.c FFT2D.c  FFT2D_Init.c

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
MBED_FLAGS += -DFFTR2_128 -DFFTR4_256
INC_PATH += -I$(TILER_PATH)/StdTypes -I$(TILER_GENERATOR_PATH)/FFT_Lib

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_PATH)/GenTiling -I$(TILER_GENERATOR_PATH)/FFT_Lib -I$(TILER_PATH)/StdTypes FFT2DModel.c $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
FFT2D.c FFT2D_Init.c: GenTile
	./GenTile -m

model: FFT2D.c FFT2D_Init.c

clean::
	$(RM) GenTile FFT2D.*  FFT2D_Init.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

