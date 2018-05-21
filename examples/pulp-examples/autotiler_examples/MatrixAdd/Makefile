# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

pulpChip = GAP
PULP_APP = MatrixAdd
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
PULP_APP_SRCS = main.c ModelKernels.c ModelKernelsInit.c BasicKernel.c
MODEL_SRC = MatAddModel.c
pulpFc=1
RM=rm -f

PULP_CFLAGS += -O3
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns
# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
PULP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable
PULP_CFLAGS += -I$(TILER_PATH)/StdTypes -I$(TILER_GENERATOR_PATH)/CnnStdModel

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc -o GenTile -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes/ $(MODEL_SRC) $(TILER_PATH)/GenTiling/LibTile.a

# Run the code generator
ModelKernels.c ModelKernelsInit.c: GenTile
	./GenTile

model: ModelKernels.c ModelKernelsInit.c

clean::
	$(RM) GenTile ModelKernels.* ModelKernelsInit.*

.PHONY: model clean

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
