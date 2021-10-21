# User Test
#------------------------------------------
MFCC_DIR ?= $(AT_HOME)/DSP_Generators
MFCCBUILD_DIR ?= $(CURDIR)/BUILD_MODEL
MFCC_MODEL_GEN = $(MFCCBUILD_DIR)/GenMFCC
MFCC_SRCG += $(MFCC_DIR)/DSP_Generators.c
FFT_LUT = $(MFCCBUILD_DIR)/LUT.def
MFCC_LUT = $(MFCCBUILD_DIR)/MFCC_FB.def
MFCC_PARAMS_JSON = MfccConfig.json

# Everything bellow is not application specific
TABLE_CFLAGS=-lm

#SDL_FLAGS= -lSDL2 -lSDL2_ttf -DAT_DISPLAY
CLUSTER_STACK_SIZE?=2048
CLUSTER_SLAVE_STACK_SIZE?=1024

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
	TOTAL_STACK_SIZE = $(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 8)
	MODEL_L1_MEMORY=$(shell expr 125000 \- $(TOTAL_STACK_SIZE))
else
	TOTAL_STACK_SIZE = $(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
	MODEL_L1_MEMORY=$(shell expr 60000 \- $(TOTAL_STACK_SIZE))
endif
ifdef MODEL_L1_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L1 $(MODEL_L1_MEMORY)
endif
ifdef MODEL_L2_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L2 $(MODEL_L2_MEMORY)
endif
ifdef MODEL_L3_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L3 $(MODEL_L3_MEMORY)
endif
USE_DISP=0
ifdef USE_DISP
  SDL_FLAGS= -lSDL2 -lSDL2_ttf
else
  SDL_FLAGS=
endif

USE_POWER?=0

$(MFCCBUILD_DIR):
	mkdir $(MFCCBUILD_DIR)

# Build the code generator from the model code
$(MFCC_MODEL_GEN): $(MFCCBUILD_DIR)
	gcc -g -o $(MFCC_MODEL_GEN) -I. -I$(MFCC_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) $(CURDIR)/MfccModel.c $(MFCC_SRCG) $(TILER_LIB) $(TABLE_CFLAGS) $(COMPILE_MODEL_EXTRA_FLAGS) -DUSE_POWER=$(USE_POWER) $(SDL_FLAGS)

$(MFCC_LUT): $(MFCCBUILD_DIR)
	python $(AT_HOME)/DSP_Libraries/LUT_Tables/gen_scripts/GenMFCCLUT.py --fft_lut_file $(FFT_LUT) --mfcc_bf_lut_file $(MFCC_LUT) --params_json $(MFCC_PARAMS_JSON) --save_params_header MFCC_params.h

# Run the code generator  kernel code
$(MFCCBUILD_DIR)/MFCCKernels.c: $(MFCC_LUT) $(MFCC_MODEL_GEN)
	$(MFCC_MODEL_GEN) -o $(MFCCBUILD_DIR) -c $(MFCCBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

gen_mfcc_code: $(MFCCBUILD_DIR)/MFCCKernels.c

clean_mfcc_code:
	rm -rf $(MFCCBUILD_DIR)
