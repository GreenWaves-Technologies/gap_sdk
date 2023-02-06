# User Test
#------------------------------------
ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

MINDIM ?= 64
MAXDIM ?= 2048
DATA_TYPE ?= -1
PAD ?= 0

APP              = test
APP_SRCS        += FFTRunTest.c $(AT_HOME)/DSP_Libraries/FFT_Library.c $(AT_HOME)/DSP_Libraries/LUT_Tables/TwiddlesDef.c $(AT_HOME)/DSP_Libraries/LUT_Tables/SwapTablesDef.c 
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I. -I$(AT_HOME)/DSP_Libraries/ -I$(AT_HOME)/DSP_Libraries/LUT_Tables
APP_CFLAGS 		+= -I$(FFT_BUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -DPERF #-DPRINTDEB
APP_CFLAGS 		+= -DMAXDIM=$(MAXDIM) -DMINDIM=$(MINDIM) -DDATA_TYPE=$(DATA_TYPE)

INDATA_DEF = In_Data.h
$(INDATA_DEF):
	gcc -o GenInData InitData.c -I. -I$(TILER_EMU_INC) -lm -DGENERATE_FILES -DMAXDIM=$(MAXDIM) -DPAD=$(PAD)
	./GenInData

init_data: $(INDATA_DEF)

clean_data:
	rm -f $(INDATA_DEF)
	rm -f GenInData

all:: init_data

clean:: clean_data

include $(RULES_DIR)/pmsis_rules.mk
