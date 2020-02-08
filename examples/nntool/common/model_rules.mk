# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifdef TRAIN_7BIT
  MODEL_TRAIN_FLAGS = -c
else
  MODEL_TRAIN_FLAGS =
endif

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

$(MODEL_TRAIN_BUILD):
	mkdir $(MODEL_TRAIN_BUILD)

$(MODEL_BUILD):
	mkdir $(MODEL_BUILD)	

ifneq ("$(wildcard $(MODEL_TRAIN))","")
# Runs the Keras script to create and train the model
# Exports the graph and trained tensors in H5 format
$(MODEL_H5): $(MODEL_TRAIN) | $(MODEL_TRAIN_BUILD)
	echo "CREATING AND TRAINING KERAS MODEL"
	$(MODEL_PYTHON) $(MODEL_TRAIN) $(MODEL_TRAIN_FLAGS) -e $(TRAINING_EPOCHS) $@

# PHONY targets defined for each step so that you can execute in sequence to
# demonstrate the flow
train: $(MODEL_H5)

# Converts the H5 file to TFLITE format
$(MODEL_TFLITE): $(MODEL_H5) | $(MODEL_BUILD)
	echo "CONVERTING KERAS H5 TO TENSORFLOW LITE FLATBUFFER"
	$(MODEL_PYTHON) model/h5_to_tflite.py $< $@

tflite: $(MODEL_TFLITE)
else
$(MODEL_TFLITE): $(TRAINED_TFLITE_MODEL) | $(MODEL_BUILD)
	cp $< $@
endif

# Creates an NNTOOL state file by running the commands in the script
# These commands could be run interactively
# The commands:
# 	Adjust the model to match AutoTiler tensor order
#	Fuse nodes together to match fused AutoTiler generators
#	Auto quantify the graph
#	Save the graph state files

$(MODEL_STATE): $(MODEL_TFLITE) $(IMAGES) $(NNTOOL_SCRIPT) | $(MODEL_BUILD)
	echo "GENERATING NNTOOL STATE FILE"
	$(NNTOOL) -s $(NNTOOL_SCRIPT) $<

nntool_state: $(MODEL_STATE)

# Runs NNTOOL with its state file to generate the autotiler model code
$(MODEL_BUILD)/$(MODEL_SRC): $(MODEL_STATE) $(MODEL_TFLITE) | $(MODEL_BUILD)
	echo "GENERATING AUTOTILER MODEL"
	$(NNTOOL) -g -M $(MODEL_BUILD) -m $(MODEL_SRC) -T $(TENSORS_DIR) $(MODEL_GENFLAGS_EXTRA) $<

nntool_gen: $(MODEL_BUILD)/$(MODEL_SRC)

# Build the code generator from the model code
$(MODEL_GEN_EXE): $(MODEL_BUILD)/$(MODEL_SRC) $(EXTRA_GENERATOR_SRC) | $(MODEL_BUILD)
	echo "COMPILING AUTOTILER MODEL"
	gcc -g -o $(MODEL_GEN_EXE) -I. -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(TILER_CNN_GENERATOR_PATH) $(MODEL_BUILD)/$(MODEL_SRC) $(TILER_CNN_GENERATOR_PATH)/CNN_Generators.c $(EXTRA_GENERATOR_SRC) $(TILER_LIB) $(SDL_FLAGS)

compile_model: $(MODEL_GEN_EXE)

# Run the code generator to generate GAP graph and kernel code
$(MODEL_GEN_C): $(MODEL_GEN_EXE)
	echo "RUNNING AUTOTILER MODEL"
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)

# A phony target to simplify including this in the main Makefile
model: $(MODEL_GEN_C)

clean_model:
	$(RM) $(MODEL_GEN_EXE)
	$(RM) -rf $(MODEL_BUILD)
	$(RM) *.dat

clean_train:
	$(RM) -rf $(MODEL_TRAIN_BUILD)

clean_images:
	$(RM) -rf $(IMAGES)

test_images: $(IMAGES)

.PHONY: model clean_model clean_train test_images clean_images train nntool_gen nntool_state tflite compile_model
