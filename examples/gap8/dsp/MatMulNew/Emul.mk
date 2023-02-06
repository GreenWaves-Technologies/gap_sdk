# User Test
#------------------------------------
COMPILE_MODEL_EXTRA_FLAGS += -D__FLOAT_EMUL__
W_M1 ?= 50
H_M1 ?= 50
W_M2 ?= 50
include MatMulModel.mk
include $(RULES_DIR)/at_common_decl.mk
BUILD_DIR = BUILD_EMUL

PMSIS_OS?=pulpos
APP          = test
SRCS        += MatMulRunTest.c $(MATMUL_SRC_CODE) $(MODEL_LIB_DSP)
INCLUDES    += -I$(TILER_DSP_KERNEL_PATH) -I$(MATMULBUILD_DIR) -I$(TILER_INC) -I$(TILER_EMU_INC)
LIBS		+= -lm
CC = gcc
CFLAGS      += -ggdb -O0 -D__EMUL__ -DPRINT_INOUT -DPRINTDEB -D__FLOAT_EMUL__ $(MATMUL_OPT_FLAG)
EXE = matmul_emul

OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: gen_matmul_code $(EXE) 

$(OBJS) : $(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -MD -MF $(basename $@).d -o $@

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -MMD -MP $(CFLAGS) $(INCLUDES) -o $(EXE) $(OBJS) $(LFLAGS) $(LIBS)

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(EXE)