# User Test
#------------------------------------------
MELBUILD_DIR 		 ?= $(CURDIR)/BUILD_DSP
MEL_MODEL_GEN 	  = $(MELBUILD_DIR)/GenMel
MEL_SRCG 				 += $(TILER_DSP_GENERATOR_PATH)/DSP_Generators.c
ifeq ($(FIXED_POINT), 1)
MEL_PARAMS_JSON 	= MelConfigFix16.json
else
MEL_PARAMS_JSON 	= MelConfig.json
endif
MEL_PARAM_HEADER	= $(MELBUILD_DIR)/MEL_PARAMS.h
MEL_SRC_CODE 			= $(MELBUILD_DIR)/MelKernels.c

# Everything bellow is not application specific
TABLE_CFLAGS=-lm
CLUSTER_STACK_SIZE?=2048
CLUSTER_SLAVE_STACK_SIZE?=1024
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
	TOTAL_STACK_SIZE = $(shell expr $(CLUSTER_STACK_SIZE) \+ $(CLUSTER_SLAVE_STACK_SIZE) \* 8)
	MODEL_L1_MEMORY=$(shell expr 128000 \- $(TOTAL_STACK_SIZE))
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

$(MELBUILD_DIR):
	mkdir $(MELBUILD_DIR)

$(MEL_PARAM_HEADER): | $(MELBUILD_DIR)
	python $(TILER_DSP_GENERATOR_PATH)/DSP_LUTGen.py $(MEL_PARAMS_JSON) --build_dir $(MELBUILD_DIR) --save_params_header $(MEL_PARAM_HEADER) --save_text

# Build the code generator from the model code
$(MEL_MODEL_GEN): $(MEL_PARAM_HEADER) | $(MELBUILD_DIR)
	gcc -g -o $(MEL_MODEL_GEN) -I. -I$(TILER_DSP_GENERATOR_PATH) -I$(MELBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) $(CURDIR)/MelModel.c $(MEL_SRCG) $(TILER_LIB) $(TABLE_CFLAGS) 

# Run the code generator  kernel code
$(MEL_SRC_CODE): $(MEL_MODEL_GEN) | $(MELBUILD_DIR)
	$(MEL_MODEL_GEN) -o $(MELBUILD_DIR) -c $(MELBUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

gen_mel_code: $(MEL_SRC_CODE)

clean_mel_code:
	rm -rf $(MELBUILD_DIR)

.PHONY: gen_mel_code clean_mel_code
