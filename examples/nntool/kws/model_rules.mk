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

ifdef MODEL_SQ8
  CNN_GEN = $(MODEL_GEN_SQ8)
  CNN_GEN_INCLUDE = $(MODEL_GEN_INCLUDE_SQ8)
  CNN_LIB = $(MODEL_LIB_SQ8)
  CNN_LIB_INCLUDE = $(MODEL_LIB_INCLUDE_SQ8)
else
  CNN_GEN = $(MODEL_GEN_POW2)
  CNN_GEN_INCLUDE = $(MODEL_GEN_INCLUDE_POW2)
  CNN_LIB = $(MODEL_LIB_POW2)
  CNN_LIB_INCLUDE = $(MODEL_LIB_INCLUDE_POW2)
endif

USE_DISP=1

ifdef USE_DISP
  SDL_FLAGS= -lSDL2 -lSDL2_ttf
else
  SDL_FLAGS=
endif

$(MODEL_TRAIN_BUILD):
	mkdir $(MODEL_TRAIN_BUILD)

$(MODEL_BUILD):
	mkdir $(MODEL_BUILD)	

# Runs the Keras script to create and train the model
# Exports the graph and trained tensors in H5 format
$(MODEL_TF): $(MODEL_TRAIN) | $(MODEL_TRAIN_BUILD)
	echo "CREATING AND TRAINING TENSORFLOW MODEL"
	$(MODEL_PYTHON) $(MODEL_TRAIN) $(MODEL_TRAIN_FLAGS) --how_many_training_steps $(TRAINING_EPOCHS) --learning_rate 0.001 --train_dir $(MODEL_TRAIN_BUILD)   $@

# PHONY targets defined for each step so that you can execute in sequence to
# demonstrate the flow
train: $(MODEL_TF)

# Converts the TF file to TFLITE format, generate feature files and corresponding CNN outputs
$(MODEL_TFLITE): $(MODEL_TF) | $(MODEL_BUILD)
	echo "CONVERTING TENSORFLOW  TO TENSORFLOW LITE FLATBUFFER"
#	$(MODEL_PYTHON)  $(MODEL_FREEZE) --start_checkpoint=$(MODEL_TRAIN_BUILD)/conv.ckpt-18000    --output_file=$(MODEL_TRAIN_BUILD)/kws_frozen.pb
	$(MODEL_PYTHON)  $(MODEL_FREEZE) --start_checkpoint=$(MODEL_TRAIN_BUILD)/conv.ckpt-10    --output_file=$(MODEL_TRAIN_BUILD)/kws_frozen.pb
	tflite_convert --graph_def_file=$(MODEL_TRAIN_BUILD)/kws_frozen.pb --output_file=$(MODEL_BUILD)/kws.tflite --input_format=TENSORFLOW_GRAPHDEF   --output_format=TFLITE --input_arrays=Reshape --output_arrays=add_2

tflite: $(MODEL_TFLITE)

# Creates an NNTOOL state file by running the commands in the script
# These commands could be run interactively
# The commands:
# 	Adjust the model to match AutoTiler tensor order
#	Fuse nodes together to match fused AutoTiler generators
#	Auto quantify the graph
#	Save the graph state files

$(IMAGES):
	echo "GENERATING INPUT IMAGES"
	(mkdir -p $(IMAGES);	$(MODEL_PYTHON) ./model/save_samples.py --batch_size 5 --start_checkpoint $(MODEL_TRAIN_BUILD)/conv.ckpt-10)

$(MODEL_STATE): $(MODEL_TFLITE) $(IMAGES)
	echo "GENERATING NNTOOL STATE FILE"
	$(NNTOOL) -s $(NNTOOL_SCRIPT) $<

nntool_state: $(MODEL_STATE)

# Runs NNTOOL with its state file to generate the autotiler model code
$(MODEL_BUILD)/$(MODEL_SRC): $(MODEL_STATE) $(MODEL_TFLITE) | $(MODEL_BUILD)
	echo "GENERATING AUTOTILER MODEL"
	$(NNTOOL) -g -M $(MODEL_BUILD) -m $(MODEL_SRC) -T $(TENSORS_DIR) $(MODEL_GENFLAGS_EXTRA) $<

nntool_gen: $(MODEL_BUILD)/$(MODEL_SRC)

# Build the code generator from the model code
$(MODEL_GEN_EXE): $(CNN_GEN) $(MODEL_BUILD)/$(MODEL_SRC) $(EXTRA_GENERATOR_SRC) | $(MODEL_BUILD)
	echo "COMPILING AUTOTILER MODEL"
	gcc -g -o $(MODEL_GEN_EXE) -I. -I$(TILER_INC) -I$(TILER_EMU_INC) $(CNN_GEN_INCLUDE) $(CNN_LIB_INCLUDE) $? $(TILER_LIB)

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
	$(RM) $(MODEL_BUILD)/*.dat

clean_train:
	$(RM) -rf $(MODEL_TRAIN_BUILD)

clean_images:
	$(RM) -rf $(IMAGES)

test_images: $(IMAGES)

.PHONY: model clean_model clean_train test_images clean_images train nntool_gen nntool_state tflite compile_model
