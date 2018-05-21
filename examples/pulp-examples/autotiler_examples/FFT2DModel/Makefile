# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = TestFFT2D
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_GENERATOR_PATH=$(TILER_PATH)/autotiler_generator
PULP_APP_SRCS = TestFFT2D.c $(TILER_GENERATOR_PATH)/FFT_Lib/FFT_Lib.c $(TILER_GENERATOR_PATH)/FFT_Lib/FFT_Scalar_Lib.c $(TILER_GENERATOR_PATH)/FFT_Lib/TwiddlesDef.c $(TILER_GENERATOR_PATH)/FFT_Lib/SwapTablesDef.c FFT2D.c  FFT2D_Init.c
pulpFc=1
RM=rm -f

PULP_CFLAGS += -O3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns  -fdata-sections -ffunction-sections
PULP_LDFLAGS +=  -flto -Wl,--gc-sections
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Wno-maybe-uninitialized -Wno-unused-but-set-variable
PULP_CFLAGS += -I$(TILER_PATH)/StdTypes -I$(TILER_GENERATOR_PATH)/FFT_Lib/ -DFFTR2_128 -DFFTR4_256

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_PATH)/GenTiling -I$(TILER_GENERATOR_PATH)/FFT_Lib -I$(TILER_PATH)/StdTypes FFT2DModel.c $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
FFT2D.c FFT2D_Init.c: GenTile
	./GenTile

model: FFT2D.c FFT2D_Init.c

clean::
	$(RM) GenTile FFT2D.*  FFT2D_Init.*

.PHONY: model clean



include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk

