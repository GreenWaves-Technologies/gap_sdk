# User Test
#------------------------------------
RESIZE_DIR ?= $(TILER_BILINEAR_RESIZE_KERNEL_PATH)
RESIZE_BUILD_DIR ?= $(CURDIR)/BUILD_MODEL
RESIZE_GEN = $(RESIZE_BUILD_DIR)/GenMFCC
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

# Compile and run Autotiler model

$(RESIZE_BUILD_DIR):
	mkdir $(RESIZE_BUILD_DIR)

# Build the code generator from the model code
$(RESIZE_GEN): $(RESIZE_BUILD_DIR)
	gcc -g -o $(RESIZE_GEN) -I. -I$(TILER_INC) -I$(TILER_EMU_INC) ResizeGenTest.c $(TILER_BILINEAR_RESIZE_KERNEL_PATH)/ResizeGenerator.c $(TILER_LIB) #$(SDL_FLAGS)

resize_model: $(RESIZE_GEN)
	$(RESIZE_GEN) -o $(RESIZE_BUILD_DIR) -c $(RESIZE_BUILD_DIR) $(MODEL_GEN_EXTRA_FLAGS)

clean_model:
	rm -rf $(RESIZE_BUILD_DIR)

# Compile and run application
APP              = Resizer
APP_SRCS        += ResizeRunTest.c $(RESIZE_BUILD_DIR)/ResizeKernels.c $(TILER_BILINEAR_RESIZE_KERNEL_PATH)/ResizeBasicKernels.c $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_INC	        += 
APP_LDFLAGS		+= 
LIBS 		    +=
APP_CFLAGS      += -O3 -I$(TILER_BILINEAR_RESIZE_KERNEL_PATH) -I$(GAP_LIB_PATH)/include
APP_CFLAGS 		+= -I$(RESIZE_BUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC)

all:: resize_model

clean:: clean_model

include $(RULES_DIR)/pmsis_rules.mk
