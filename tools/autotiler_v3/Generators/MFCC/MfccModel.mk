# User Test
#------------------------------------------
MFCC_DIR ?= $(CURDIR)
MFCCBUILD_DIR ?= $(CURDIR)/BUILD_MODEL
MFCC_MODEL_GEN = $(MFCCBUILD_DIR)/GenMFCC
MFCC_SRCG += $(MFCC_DIR)/MfccGenerator.c

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

$(MFCCBUILD_DIR):
	mkdir $(MFCCBUILD_DIR)

# Build the code generator from the model code
$(MFCC_MODEL_GEN): $(MFCCBUILD_DIR)
	gcc -g -o $(MFCC_MODEL_GEN) -I. -I$(TILER_INC) -I$(TILER_EMU_INC) $(MFCC_DIR)/MfccModel.c $(MFCC_SRCG) $(TILER_LIB)  $(TABLE_CFLAGS) #$(SDL_FLAGS)

gen_lut: $(MFCCBUILD_DIR)
	python GenLUT.py --fft_lut_file $(MFCCBUILD_DIR)/LUT.def --mfcc_bf_lut_file $(MFCCBUILD_DIR)/MFCC_FB.def \
					 --sample_rate 16000 --frame_size 480 --frame_step 160 --n_frame 49 --n_fft 512 --n_dct 40 \
					 --mfcc_bank_cnt 40 --fmin 20 --fmax 4000 --use_power --mfcc_bank_cnt 40 --preempfactor 0.0 \
					 --save_params_header MFCC_params.h

# Run the code generator  kernel code
mfcc_model: gen_lut $(MFCC_MODEL_GEN)
	$(MFCC_MODEL_GEN) -o $(MFCCBUILD_DIR) -c $(MFCCBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

clean_model:
	rm -rf $(MFCCBUILD_DIR)
