# User Test
#------------------------------------
COMPILE_MODEL_EXTRA_FLAGS += -DNO_FLOAT16
include MfccModel.mk
BUILD_DIR = BUILD_EMUL

PMSIS_OS?=pulpos
APP          = test
SRCS        += MfccRunTest.c $(GAP_LIB_PATH)/wav_io/wavIO.c $(MFCCBUILD_DIR)/MFCCKernels.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/TwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/RFFTTwiddlesDef.c $(TILER_DSP_KERNEL_PATH)/LUT_Tables/SwapTablesDef.c
SRCS 		+= $(TILER_DSP_KERNEL_PATH)/MfccBasicKernels.c $(TILER_DSP_KERNEL_PATH)/FFT_Library.c $(TILER_DSP_KERNEL_PATH)/math_funcs.c $(TILER_DSP_KERNEL_PATH)/CmplxFunctions.c $(TILER_DSP_KERNEL_PATH)/PreProcessing.c 
INCLUDES    += -I. -I$(TILER_DSP_KERNEL_PATH) -I$(TILER_DSP_KERNEL_PATH)/LUT_Tables -I$(GAP_SDK_HOME)/libs/gap_lib/include -I$(MFCCBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC)
LIBS		+= -lm
CC = gcc
CFLAGS      += -ggdb -O0 -D__EMUL__ -DPRINT_INOUT -DPRINTDEB
EXE = mfcc_emul

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(MFCCBUILD_DIR)/MFCCKernels.c $(EXE)

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(EXE) $(OBJS) $(LFLAGS) $(LIBS)

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(EXE)