# User Test
#------------------------------------
EMUL = 1
include FFTModel.mk
BUILD_DIR = BUILD_EMUL

PMSIS_OS?=pulpos
APP          = test
SRCS        += FFTRunTest.c $(FFT_BUILD_DIR)/RFFTKernels.c $(GAP_LIB_PATH)/wav_io/wavIO.c
SRCS 		+= $(TILER_DSP_KERNEL_PATH)/FFT_Library.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/TwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/RFFTTwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/SwapTablesDef.c 
SRCS 		+= $(TILER_DSP_KERNEL_PATH)/PreProcessing.c $(TILER_DSP_KERNEL_PATH)/CmplxFunctions.c $(TILER_DSP_KERNEL_PATH)/math_funcs.c
CFLAGS      += -I. -I$(TILER_DSP_KERNEL_PATH) -I$(TILER_DSP_KERNEL_PATH)/LUT_Tables -DN_FFT=$(NFFT)
CFLAGS 		+= -I$(FFT_BUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(GAP_SDK_HOME)/libs/gap_lib/include -DPRINTDEB

LIBS		+= -lm
CC = gcc
CFLAGS      += -ggdb -O0 -D__EMUL__
EXE = fft_emul

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(FFT_GEN_SRC) $(EXE)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(EXE) $(OBJS) $(LFLAGS) $(LIBS)

clean: clean_fft_code
	$(RM) -r $(BUILD_DIR)
	$(RM) $(EXE)