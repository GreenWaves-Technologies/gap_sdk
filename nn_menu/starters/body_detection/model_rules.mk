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

# Converts the H5 file to TFLITE format
$(MODEL_TFLITE): $(MODEL_BUILD)
	cp $(MODEL_TFLITE) $(MODEL_BUILD)


$(MODEL_STATE): $(MODEL_TFLITE)  
	$(NNTOOL) -s $(NNTOOL_SCRIPT) $(MODEL_BUILD)/$(MODEL_NAME)

# Runs NNTOOL with its state file to generate the autotiler model code
$(MODEL_BUILD)/$(MODEL_SRC): $(MODEL_STATE) $(MODEL_TFLITE)
	$(NNTOOL) -g -M $(MODEL_BUILD) -m $(MODEL_SRC) -T $(TENSORS_DIR) -H $(MODEL_HEADER) $(MODEL_GENFLAGS_EXTRA) $<

# Build the code generator from the model code
$(MODEL_GEN_EXE): $(MODEL_BUILD)/$(MODEL_SRC)
	#gcc -g -o $(MODEL_GEN_EXE) -I$(TILER_INC) -I$(GEN_PATH) $(MODEL_BUILD)/$(MODEL_SRC) $(CNN_GEN) $(TILER_LIB) # -lSDL2 -lSDL2_ttf
	gcc -g -o $(MODEL_GEN_EXE) -I. -I$(TILER_INC) -I$(TILER_EMU_INC) $(CNN_GEN_INCLUDE) $(CNN_LIB_INCLUDE) $(MODEL_BUILD)/$(MODEL_SRC) $(CNN_GEN) $(TILER_LIB)

# Run the code generator to generate GAP graph and kernel code
$(MODEL_GEN_C): $(MODEL_GEN_EXE)
	$(MODEL_GEN_EXE) -o $(MODEL_BUILD) -c $(MODEL_BUILD) $(MODEL_GEN_EXTRA_FLAGS)
	
model: $(MODEL_GEN_C)

clean_model:
	$(RM) $(MODEL_GEN_EXE)
	$(RM) -rf $(MODEL_BUILD)
	$(RM) *.dat



.PHONY: model clean_model  
