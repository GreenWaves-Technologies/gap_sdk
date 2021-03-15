# User Test
#------------------------------------
WAV_FILE = $(CURDIR)/samples/yes.wav
USE_ABS ?= 0
USE_HIGH_PREC ?= 0

include MfccModel.mk

APP              = test
APP_SRCS        += MfccRunTest.c $(GAP_LIB_PATH)/wav_io/wavIO.c $(MFCCBUILD_DIR)/MFCCKernels.c MfccBasicKernels.c FFTLib.c
APP_INC	        += 
APP_LDFLAGS		+= 
LIBS 		    += -lm
APP_CFLAGS      += -O3 -I. -I$(GAP_SDK_HOME)/libs/gap_lib/include/gaplib -DAT_WAV=$(WAV_FILE)
APP_CFLAGS 		+= -I$(MFCCBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -DPERF -DPRINT_INOUT #-DPRINTDEB -DPRINT_WAV

ifeq ($(USE_ABS), 1)
	APP_CFLAGS += -DUSE_ABS
endif
ifeq ($(USE_HIGH_PREC), 1)
	APP_CFLAGS += -DHIGH_PREC_FFT
endif

all:: mfcc_model

clean:: clean_model

include $(RULES_DIR)/pmsis_rules.mk
