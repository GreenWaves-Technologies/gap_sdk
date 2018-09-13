# User Test
#------------------------------------------
TILER_PATH = $(GAP_SDK_HOME)/tools/autotiler
TILER_GENERATOR_PATH=$(TILER_PATH)/autotiler_generator
TEST_C          = ./MFCC/MFCC_Processing.c ./MFCC/MFCC.c ./AUDIO/AUDIO.c ./test_KWS.c \
		./CNN_KERNELS/CnnKernels.c  ./CNN_KERNELS/CnnKernels_tileddense.c \
		./CNN_KERNELS/CNN_BasicKernels.c ./CNN_KERNELS/CNN_HwCE.c \

INC            += ./MFCC ./AUDIO ./WEIGHTS_BIAS ./CNN_KERNELS ./include

# If not use HWCE
# TEST_C         +=./CNN_KERNELS/CnnKernelsInit.c

# If use HWCE
USE_HARDWARE_CE = -DRT_HAS_HWCE=1
TEST_C         +=./CnnKernels_hwce.c ./CnnKernelsInit_hwce.c

USE_MFCC        = -DDOMFCC=1
USE_AUDIO       = -DUSE_AUDIO=1

MBED_FLAGS     += -DW_HALFCHAR -DTILED -DHWCE_ACTIVE $(USE_HARDWARE_CE) $(USE_MFCC) $(USE_AUDIO) -lm

MODEL_SRC = ./CNN_KERNELS/KwsGenerator.c

# The double colon allows us to force this to occur before the imported all target
# Link model generation to all step
all:: model

# Build the code generator
GenTile:
	gcc $(USE_HARDWARE_CE)  -o GenTile -I$(TILER_PATH)/GenTiling -I$(TILER_PATH)/StdTypes -I./include $(MODEL_SRC) ./CNN_KERNELS/CNN_Generator.c $(TILER_PATH)/GenTiling/LibTile.a

#$(TILER_GENERATOR_PATH)/CnnStdModel/CNN_Generator.c

# Run the code generator
model: GenTile
	./GenTile -m

clean::
	rm -rf GenTile CnnKernels_hwce.* CnnKernelsInit_hwce.*

.PHONY: model

# If use AUDIO, uncomment it.
PLPBRIDGE_FLAGS += -s i2s@../../../i2s.py

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
