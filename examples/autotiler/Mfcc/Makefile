# User Test
#------------------------------------
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP8'
	COMPILE_MODEL_EXTRA_FLAGS += -DNO_FLOAT16
endif
WAV_FILE = $(CURDIR)/samples/yes.wav
include MfccModel.mk

PMSIS_OS?=pulpos
APP              = test
APP_SRCS        += MfccRunTest.c $(GAP_LIB_PATH)/wav_io/wavIO.c $(MFCCBUILD_DIR)/MFCCKernels.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/TwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/RFFTTwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/SwapTablesDef.c
APP_SRCS  		+= $(TILER_DSP_KERNEL_PATH)/MfccBasicKernels.c $(TILER_DSP_KERNEL_PATH)/FFT_Library.c $(TILER_DSP_KERNEL_PATH)/math_funcs.c $(TILER_DSP_KERNEL_PATH)/CmplxFunctions.c $(TILER_DSP_KERNEL_PATH)/PreProcessing.c 
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I. -I$(TILER_DSP_KERNEL_PATH) -I$(TILER_DSP_KERNEL_PATH)/LUT_Tables -I$(GAP_SDK_HOME)/libs/gap_lib/include -DAT_WAV=$(WAV_FILE)
APP_CFLAGS 		+= -I$(MFCCBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -DPERF -DPRINT_INOUT #-DPRINTDEB

all:: $(MFCCBUILD_DIR)/MFCCKernels.c

include $(RULES_DIR)/pmsis_rules.mk
