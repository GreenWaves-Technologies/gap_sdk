# User Test
#------------------------------------------
MFCC_GENERATOR ?= $(TILER_DSP_GENERATOR_PATH)
MFCCBUILD_DIR ?= $(CURDIR)/BUILD_MFCC_MODEL
MFCC_MODEL_GEN = $(MFCCBUILD_DIR)/GenMFCC
FFT_LUT = $(MFCCBUILD_DIR)/LUT.def
MFCC_LUT = $(MFCCBUILD_DIR)/MFCC_FB.def
ifeq ($(USE_HIGH_PREC), 1)
	MFCC_PARAMS_JSON = MfccConfig_HighPrec.json
else
	MFCC_PARAMS_JSON = MfccConfig.json
endif

#SDL_FLAGS= -lSDL2 -lSDL2_ttf -DAT_DISPLAY
ifdef MODEL_L1_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L1 $(MODEL_L1_MEMORY)
endif
ifdef MODEL_L2_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L2 $(MODEL_L2_MEMORY)
endif
ifdef MODEL_L3_MEMORY
  MODEL_GEN_EXTRA_FLAGS += --L3 $(MODEL_L3_MEMORY)
endif

$(MFCCBUILD_DIR):
	mkdir $(MFCCBUILD_DIR)

# Build the code generator from the model code
$(MFCC_MODEL_GEN): $(MFCCBUILD_DIR)
	gcc -g -o $(MFCC_MODEL_GEN) -I. -I$(MFCCBUILD_DIR) -I$(TILER_DSP_GENERATOR_PATH) -I$(TILER_INC) -I$(TILER_EMU_INC) $(CURDIR)/MFCCmodel.c $(TILER_DSP_GENERATOR_PATH)/DSP_Generators.c $(TILER_LIB) -DUSE_POWER=$(USE_POWER) -lm

$(MFCC_LUT): $(MFCCBUILD_DIR)
	python $(TILER_MFCC_GEN_LUT_SCRIPT) --fft_lut_file $(FFT_LUT) --mfcc_bf_lut_file $(MFCC_LUT) --params_json $(MFCC_PARAMS_JSON) --save_params_header MFCC_params.h

# Run the code generator  kernel code
$(MFCCBUILD_DIR)/MFCCKernels.c: $(MFCC_LUT) $(MFCC_MODEL_GEN)
	$(MFCC_MODEL_GEN) -o $(MFCCBUILD_DIR) -c $(MFCCBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

clean_mfcc_code:
	rm -rf $(MFCCBUILD_DIR)
