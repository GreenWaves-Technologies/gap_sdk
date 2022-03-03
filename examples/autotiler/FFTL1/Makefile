# User Test
#------------------------------------

#PMSIS_OS?=pulpos
APP              = test
APP_SRCS        += FFTRunTest.c $(AT_HOME)/DSP_Libraries/FFT_Library.c $(AT_HOME)/DSP_Libraries/LUT_Tables/TwiddlesDef.c $(AT_HOME)/DSP_Libraries/LUT_Tables/SwapTablesDef.c 
APP_INC	        += 
APP_LDFLAGS		+= -lm
APP_CFLAGS      += -O3 -I. -I$(AT_HOME)/DSP_Libraries/ -I$(AT_HOME)/DSP_Libraries/LUT_Tables
APP_CFLAGS 		+= -I$(FFT_BUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -DPERF #-DPRINTDEB

INDATA_DEF = In_Data.h
$(INDATA_DEF):
	gcc -o GenInData InitData.c -I. -I$(TILER_EMU_INC) -lm -DGENERATE_FILES
	./GenInData

all:: $(INDATA_DEF)

clean::
	rm -f $(INDATA_DEF)
	rm -f GenInData

include $(RULES_DIR)/pmsis_rules.mk
