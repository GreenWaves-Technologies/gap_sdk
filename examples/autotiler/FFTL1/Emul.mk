# User Test
#------------------------------------
EMUL = 1
BUILD_DIR = BUILD_EMUL

PMSIS_OS?=pulpos
APP          = test
SRCS        += FFTRunTest.c $(AT_HOME)/DSP_Libraries/FFT_Library.c 
SRCS        += $(AT_HOME)/DSP_Libraries/LUT_Tables/TwiddlesDef.c $(AT_HOME)/DSP_Libraries/LUT_Tables/RFFTTwiddlesDef.c $(AT_HOME)/DSP_Libraries/LUT_Tables/SwapTablesDef.c 
INCLUDES    += -I. -I$(AT_HOME)/DSP_Libraries/ -I$(AT_HOME)/DSP_Libraries/LUT_Tables -I$(TILER_INC) -I$(TILER_EMU_INC) #-DDEBUG -DPRINTDEB
LIBS		+= -lm
CC = gcc
CFLAGS      += -O0 -D__EMUL__ -ggdb
EXE = fft_emul

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

INDATA_DEF = In_Data.h
$(INDATA_DEF):
	gcc -o GenInData InitData.c -I. -I$(TILER_EMU_INC) -lm -DGENERATE_FILES
	./GenInData

all: $(INDATA_DEF) $(EXE)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(EXE) $(OBJS) $(LFLAGS) $(LIBS)

clean:
	$(RM) GenInData
	$(RM) $(INDATA_DEF)
	$(RM) -r $(BUILD_DIR)
	$(RM) $(EXE)
