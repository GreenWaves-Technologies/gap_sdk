# User Test
#------------------------------------------
STFT_DIR ?= $(CURDIR)
STFTBUILD_DIR ?= $(CURDIR)/BUILD_MODEL
STFT_MODEL_GEN = $(STFTBUILD_DIR)/GenSTFT
STFT_SRCG += $(STFT_DIR)/MfccGenerator.c

# Everything bellow is not application specific
TABLE_CFLAGS=-lm

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

$(STFTBUILD_DIR):
	mkdir $(STFTBUILD_DIR)

# Build the code generator from the model code
$(STFT_MODEL_GEN): $(STFTBUILD_DIR)
	gcc -g -o $(STFT_MODEL_GEN) -I. -I$(TILER_INC) -I$(TILER_EMU_INC) $(STFT_DIR)/StftModel.c $(STFT_SRCG) $(TILER_LIB)  $(TABLE_CFLAGS) #$(SDL_FLAGS)

gen_lut: $(STFTBUILD_DIR)
	python GenLUT.py --fft_lut_file $(STFTBUILD_DIR)/LUT.def --sample_rate 16000 --frame_size 480 \
					 --frame_step 160 --n_frame 49 --n_fft 512 --use_power --preempfactor 0.0     \
					 --only_stft --save_params_header STFT_params.h

# Run the code generator  kernel code
stft_model: gen_lut $(STFT_MODEL_GEN)
	$(STFT_MODEL_GEN) -o $(STFTBUILD_DIR) -c $(STFTBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

clean_model:
	rm -rf $(STFTBUILD_DIR)