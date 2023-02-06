# User Test
#------------------------------------
WAV_FILE = $(CURDIR)/samples/yes.wav
include MfccModel.mk
include $(RULES_DIR)/at_common_decl.mk

io?=host

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
	FREQ_CL?=370
	FREQ_FC?=370
	FREQ_PE?=370
else
	FREQ_CL?=175
	FREQ_FC?=250
	FREQ_PE?=250
endif

APP              = test
APP_SRCS        += MfccRunTest.c $(GAP_LIB_PATH)/wav_io/wavIO.c $(MFCC_SRC_CODE) $(MODEL_LIB_DSP)
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I$(TILER_DSP_KERNEL_PATH) -I$(GAP_SDK_HOME)/libs/gap_lib/include -DAT_WAV=$(WAV_FILE)
APP_CFLAGS 		+= -I$(MFCCBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -DPERF #-DPRINTDEB #-DPRINT_INOUT
APP_CFLAGS 		+= -DFREQ_CL=$(FREQ_CL) -DFREQ_FC=$(FREQ_FC) -DFREQ_PE=$(FREQ_PE)

TEST = 1
ifeq ($(TEST), 1)
	APP_CFLAGS += -DTEST
ground_truth.h:
	python gen_ground_truth.py samples/yes.wav
gen_gt: ground_truth.h
clean_gt:
	rm -rf ground_truth.h
else
gen_gt:
clean_gt:
endif

all:: $(MFCC_SRC_CODE) gen_gt

clean:: clean_gt clean_mfcc_code

include $(RULES_DIR)/pmsis_rules.mk
