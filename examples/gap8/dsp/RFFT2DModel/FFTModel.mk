# User Test
#------------------------------------------


FFT_BUILD_DIR ?= $(CURDIR)/BUILD_MODEL
FFT_MODEL_GEN = $(FFT_BUILD_DIR)/GenSTFT
FFT_SRCG += $(TILER_DSP_GENERATOR_PATH)/DSP_Generators.c
WIN_LUT_Q16 = $(FFT_BUILD_DIR)/WinLUT.def
WIN_LUT_F16 = $(FFT_BUILD_DIR)/WinLUT_f16.def
WIN_LUT_F32 = $(FFT_BUILD_DIR)/WinLUT_f32.def

FFT_GEN_SRC = $(FFT_BUILD_DIR)/RFFTKernels.c

SDL_FLAGS= -lSDL2 -lSDL2_ttf -DAT_DISPLAY
CLUSTER_STACK_SIZE?=2048
CLUSTER_SLAVE_STACK_SIZE?=1024
TOTAL_STACK_SIZE = $(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 7)
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
	ifndef EMUL
		GEN_FLAG = -DGEN_FLOAT16
	endif
	MODEL_L1_MEMORY=$(shell expr 125000 \- $(TOTAL_STACK_SIZE))
else
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

NFFT ?= 512

$(FFT_BUILD_DIR):
	mkdir $(FFT_BUILD_DIR)

$(WIN_LUT_Q16): $(FFT_BUILD_DIR)
	python $(TILER_MFCC_GEN_LUT_SCRIPT) --fft_lut_file $(WIN_LUT_Q16) --win_func "hanning" --name_suffix "_Fix" --dtype "int" --frame_size $(NFFT) --frame_step 160 --n_fft $(NFFT)

$(WIN_LUT_F32): $(FFT_BUILD_DIR)
	python $(TILER_MFCC_GEN_LUT_SCRIPT) --fft_lut_file $(WIN_LUT_F32) --win_func "hanning" --name_suffix "_f32" --dtype "float32" --frame_size $(NFFT) --frame_step 160 --n_fft $(NFFT)

$(WIN_LUT_F16): $(FFT_BUILD_DIR)
	python $(TILER_MFCC_GEN_LUT_SCRIPT) --fft_lut_file $(WIN_LUT_F16) --win_func "hanning" --name_suffix "_f16" --dtype "float16" --frame_size $(NFFT) --frame_step 160 --n_fft $(NFFT)

# Build the code generator from the model code
$(FFT_MODEL_GEN): $(FFT_BUILD_DIR)
	gcc -g -o $(FFT_MODEL_GEN) -I. -I$(TILER_DSP_GENERATOR_PATH) -I$(TILER_INC) -I$(TILER_EMU_INC) $(CURDIR)/FFTGenCode.c $(FFT_SRCG) $(TILER_LIB) $(GEN_FLAG) -DN_FFT=$(NFFT) $(SDL_FLAGS)

# Run the code generator  kernel code
$(FFT_GEN_SRC): $(FFT_MODEL_GEN) $(WIN_LUT_Q16) $(WIN_LUT_F32) $(WIN_LUT_F16)
	$(FFT_MODEL_GEN) -o $(FFT_BUILD_DIR) -c $(FFT_BUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)


gen_fft_code: $(FFT_GEN_SRC)

clean_fft_code:
	rm -rf $(FFT_BUILD_DIR)
