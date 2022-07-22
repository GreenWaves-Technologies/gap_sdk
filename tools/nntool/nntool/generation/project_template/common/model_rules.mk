# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit

USE_DISP=1
ifdef USE_DISP
  SDL_FLAGS= -lSDL2 -lSDL2_ttf
else
  SDL_FLAGS=
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


$(MODEL_BUILD):
	mkdir $(MODEL_BUILD)

# MODEL_AT_ONLY indicates that the NNTOOL model generation should not be run
# The model is already available at AT_MODEL_PATH

ifndef MODEL_AT_ONLY
$(MODEL_PATH): $(TRAINED_MODEL) | $(MODEL_BUILD)
	cp $< $@

# Creates an NNTOOL state file by running the commands in the script
# These commands could be run interactively
# The commands:
# 	Adjust the model to match AutoTiler tensor order
#	Fuse nodes together to match fused AutoTiler generators
#	Quantize the graph if not already done with tflite quantization
#	Save the graph state files

$(MODEL_STATE): $(MODEL_PATH) $(IMAGES) $(NNTOOL_SCRIPT) | $(MODEL_BUILD)
	echo "GENERATING NNTOOL STATE FILE"
	$(NNTOOL) -s $(NNTOOL_SCRIPT) $< $(NNTOOL_EXTRA_FLAGS)

nntool_state: $(MODEL_STATE)

# Runs NNTOOL with its state file to generate the autotiler model code

$(AT_MODEL_PATH) $(MODEL_EXPRESSIONS): $(MODEL_STATE) $(MODEL_PATH) | $(MODEL_BUILD)
	echo "GENERATING AUTOTILER MODEL"
	$(NNTOOL) -g -M $(MODEL_BUILD) -m $(MODEL_SRC) -T $(TENSORS_DIR) -H $(MODEL_HEADER) $(MODEL_GENFLAGS_EXTRA) $<
nntool_gen: $(AT_MODEL_PATH)
endif

# Build the code generator from the model code
$(MODEL_GEN_EXE): $(CNN_GEN) $(AT_MODEL_PATH) $(EXTRA_GENERATOR_SRC) | $(MODEL_BUILD)
	echo "COMPILING AUTOTILER MODEL"
	gcc -g -o $(MODEL_GEN_EXE) -I. -I$(TILER_INC) -I$(TILER_EMU_INC) $(CNN_GEN_INCLUDE) $(CNN_LIB_INCLUDE) $^ $(TILER_LIB) $(SDL_FLAGS) 

compile_model: $(MODEL_GEN_EXE)

# Run the code generator to generate GAP graph and kernel code
$(MODEL_GEN_C): $(MODEL_GEN_EXE)
	echo "RUNNING AUTOTILER MODEL - START"
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)
	echo "RUNNING AUTOTILER MODEL - FINISH"

# A phony target to simplify including this in the main Makefile
model: $(MODEL_GEN_C) $(MODEL_EXPRESSIONS)

clean_at_model:
	$(RM) $(MODEL_GEN_EXE)
	$(RM) $(MODEL_BUILD)/*.dat

clean_model:
	$(RM) -rf $(MODEL_BUILD)

clean_train:
	$(RM) -rf $(MODEL_TRAIN_BUILD)

clean_images:
	$(RM) -rf $(IMAGES)

test_images: $(IMAGES)

ifndef MODEL_AT_ONLY
.PHONY: model clean_model clean_train test_images clean_images train compile_model
else
.PHONY: model clean_model clean_train test_images clean_images train nntool_gen nntool_state compile_model
endif