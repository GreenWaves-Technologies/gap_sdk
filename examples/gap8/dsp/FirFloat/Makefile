# Copyright (C) 2022 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

APP = TestFIR
FIR_GEN_PATH = $(TILER_GENERATOR_PATH)/Fir

FLOAT?=16

ifeq ($(FLOAT), 32)
	FLOAT_PREC=-DFIR_FLOAT32=1
else
	FLOAT_PREC=-DFIR_FLOAT32=0
endif



MODEL_SRC = FirModel.c
MODEL_GEN = FirKernels 
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
RM=rm -f

APP_SRCS = main.c $(MODEL_GEN_C) $(FIR_GEN_PATH)/FirBasicKernels.c

APP_CFLAGS+= -Wall -O3
APP_CFLAGS+= -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(FIR_GEN_PATH) -I$(TILER_DSP_KERNEL_PATH) $(FLOAT_PREC)
#APP_LDFLAGS+=  -flto -lm 

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile: $(MODEL_SRC)
	gcc -o GenTile $(FLOAT_PREC) -I$(TILER_INC) -I$(FIR_GEN_PATH) $(MODEL_SRC) $(FIR_GEN_PATH)/FirGenerator.c $(TILER_LIB)

# Run the code generator
$(MODEL_GEN_C): GenTile
	./GenTile

model: $(MODEL_GEN_C)

clean::
	$(RM) GenTile $(MODEL_GEN_CLEAN)

#.PHONY: model clean

include $(RULES_DIR)/pmsis_rules.mk