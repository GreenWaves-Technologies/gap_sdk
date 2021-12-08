# User Test
#------------------------------------
include FFTModel.mk
WAV_FILE = $(CURDIR)/samples/yes.wav

io = host
DATA_TYPE ?= 0
#PMSIS_OS?=pulpos

APP              = test
APP_SRCS        += FFTRunTest.c $(FFT_GEN_SRC) $(GAP_LIB_PATH)/wav_io/wavIO.c
APP_SRCS 		+= $(TILER_DSP_KERNEL_PATH)/FFT_Library.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/TwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/RFFTTwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/SwapTablesDef.c 
APP_SRCS 		+= $(TILER_DSP_KERNEL_PATH)/PreProcessing.c $(TILER_DSP_KERNEL_PATH)/CmplxFunctions.c $(TILER_DSP_KERNEL_PATH)/math_funcs.c
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I. -I$(TILER_DSP_KERNEL_PATH) -I$(TILER_DSP_KERNEL_PATH)/LUT_Tables -DAT_WAV=$(WAV_FILE)
APP_CFLAGS 		+= -I$(FFT_BUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(GAP_SDK_HOME)/libs/gap_lib/include -DPERF -DN_FFT=$(NFFT) -DF16_DSP_BFLOAT -DDATA_TYPE=$(DATA_TYPE)

all:: gen_fft_code

clean:: clean_fft_code

include $(RULES_DIR)/pmsis_rules.mk
