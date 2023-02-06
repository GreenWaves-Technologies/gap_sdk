# User Test
#------------------------------------
COMPILE_MODEL_EXTRA_FLAGS += -D__FLOAT_EMUL__
include MelModel.mk
include $(RULES_DIR)/at_common_decl.mk
BUILD_DIR = BUILD_EMUL

APP          = test
SRCS 		+= MelRunTest.c $(GAP_LIB_PATH)/wav_io/wavIO.c $(MEL_SRC_CODE) $(MODEL_LIB_DSP)
INCLUDES    += -I. -I$(MELBUILD_DIR) -I$(TILER_DSP_KERNEL_PATH) -I$(TILER_INC) -I$(TILER_EMU_INC) -I$(GAP_SDK_HOME)/libs/gap_lib/include
LIBS		+= -lm
CC = gcc
CFLAGS      += -ggdb -O0 -D__EMUL__ -DPRINT_INOUT -D__FLOAT_EMUL__ # To emulate float16 with float32

EXE = MelEmulExe

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(EXE) $(OBJS) $(LFLAGS) $(LIBS)

all:: gen_mel_code $(EXE)

clean:: clean_mel_code
	$(RM) -r $(BUILD_DIR)
	$(RM) $(EXE)
	$(RM) GapInvMel.bin
	$(RM) GapMel.bin
