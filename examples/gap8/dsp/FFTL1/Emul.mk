# User Test
#------------------------------------
ifndef GAP_SDK_HOME
  $(error Source sourceme in gap_sdk first)
endif

EMUL = 1
BUILD_DIR = BUILD_EMUL
MINDIM ?= 64
MAXDIM ?= 2048
DATA_TYPE ?= -1
PAD ?= 0

APP          = test
SRCS        += FFTRunTest.c $(AT_HOME)/DSP_Libraries/FFT_Library.c 
SRCS        += $(AT_HOME)/DSP_Libraries/LUT_Tables/TwiddlesDef.c $(AT_HOME)/DSP_Libraries/LUT_Tables/RFFTTwiddlesDef.c $(AT_HOME)/DSP_Libraries/LUT_Tables/SwapTablesDef.c 
INCLUDES    += -I. -I$(AT_HOME)/DSP_Libraries/ -I$(AT_HOME)/DSP_Libraries/LUT_Tables -I$(TILER_INC) -I$(TILER_EMU_INC)
CFLAGS 		+= -DMAXDIM=$(MAXDIM) -DMINDIM=$(MINDIM) -DDATA_TYPE=$(DATA_TYPE) #-DPRINTDEB

LIBS		+= -lm
CC = gcc
CFLAGS      += -O0 -D__EMUL__ -ggdb
EXE = fft_emul

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

INDATA_DEF = In_Data.h
$(INDATA_DEF):
	gcc -o GenInData InitData.c -I. -I$(TILER_EMU_INC) -lm -DGENERATE_FILES -DMAXDIM=$(MAXDIM) -DPAD=$(PAD)
	./GenInData

init_data: $(INDATA_DEF)

clean_data:
	rm -f $(INDATA_DEF)
	$(RM) GenInData

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(EXE) $(OBJS) $(LFLAGS) $(LIBS)

all:: init_data $(EXE)

clean:: clean_data
	$(RM) -r $(BUILD_DIR)
	$(RM) $(EXE)
