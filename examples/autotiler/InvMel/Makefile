# User Test
#------------------------------------
WAV_FILE = $(CURDIR)/samples/yes.wav

# With GAP9 default is Floating point
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
FIXED_POINT ?= 0
else
FIXED_POINT ?= 1
endif

include MelModel.mk
include $(RULES_DIR)/at_common_decl.mk

io?=host

APP              = test
APP_SRCS         = MelRunTest.c $(GAP_LIB_PATH)/wav_io/wavIO.c $(MEL_SRC_CODE) $(MODEL_LIB_DSP)
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I. -I$(TILER_DSP_KERNEL_PATH) -I$(GAP_SDK_HOME)/libs/gap_lib/include -DAT_WAV=$(WAV_FILE)
APP_CFLAGS 		+= -I$(MELBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -DPERF -DPRINT_INOUT

all:: gen_mel_code

clean_bin:
	rm -rf GapMel.bin
	rm -rf GapInvMel.bin

clean:: clean_bin clean_mel_code

include $(RULES_DIR)/pmsis_rules.mk
