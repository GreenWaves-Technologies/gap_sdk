### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

# The training of the model is slightly different depending on
# the quantization. This is because in 8 bit mode we used signed
# 8 bit so the input to the model needs to be shifted 1 bit
ifdef TRAIN_7BIT
  MODEL_TRAIN_FLAGS = -c
else
  MODEL_TRAIN_FLAGS =
endif

# $(MODEL_TRAIN_BUILD):
# 	mkdir $(MODEL_TRAIN_BUILD)

$(MODEL_BUILD):
	mkdir $(MODEL_BUILD)	

# Runs the Keras script to create and train the model
# Exports the graph and trained tensors in H5 format
# $(MODEL_H5): $(MODEL_TRAIN) | $(MODEL_TRAIN_BUILD)
# 	$(MODEL_PYTHON) $(MODEL_TRAIN) $(MODEL_TRAIN_FLAGS) -e $(TRAINING_EPOCHS) $@

# Converts the H5 file to TFLITE format
$(MODEL_TFLITE): $(MODEL_BUILD)
	cp $(MODEL_TFLITE) $(MODEL_BUILD)


$(MODEL_STATE): $(MODEL_TFLITE)  
	$(NNTOOL) -s $(NNTOOL_SCRIPT) $(MODEL_BUILD)/$(MODEL_NAME)

# Runs NNTOOL with its state file to generate the autotiler model code
$(MODEL_BUILD)/$(MODEL_SRC): $(MODEL_STATE) $(MODEL_TFLITE)
	$(NNTOOL) -g -M $(MODEL_BUILD) -m $(MODEL_SRC) -H $(MODEL_HEADER) -T $(TENSORS_DIR) $<

# Build the code generator from the model code
$(MODEL_GEN_EXE): $(MODEL_BUILD)/$(MODEL_SRC)
	gcc -g -o $(MODEL_GEN_EXE) -I$(TILER_PATH)/Autotiler -I$(TILER_EMU_INC) -I$(TILER_INC) -I$(GEN_PATH) $(CNN_GEN_INCLUDE) $(CNN_LIB_INCLUDE) $(MODEL_BUILD)/$(MODEL_SRC) $(CNN_GEN) $(TILER_LIB)

# Run the code generator to generate GAP graph and kernel code
$(MODEL_GEN_C): $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) --L1 $(MODEL_L1_MEMORY) --L2 $(MODEL_L2_MEMORY)\
	  --L3 $(MODEL_L3_MEMORY) 

model: $(MODEL_GEN_C)

clean_model:
	$(RM) $(MODEL_GEN_EXE)
	$(RM) -rf $(MODEL_BUILD)
	$(RM) *.dat

#clean_train:
#	$(RM) -rf $(MODEL_TRAIN_BUILD)

clean_images:
	$(RM) -rf $(IMAGES)

test_images: $(IMAGES)

# Dumps weights and outputs for functional tests and validation in tool
test_files: $(MODEL_TFLITE)
	python model/run.py $(MODEL_H5) -i ../examples/0/136.pgm -D ../tests/h5_pickles -d
	python model/run.py $(MODEL_H5) -D ../tests/h5_pickles -w weights.pickle
	cp $(MODEL_TFLITE) ../examples

.PHONY: model clean_model clean_train test_files test_images clean_images
