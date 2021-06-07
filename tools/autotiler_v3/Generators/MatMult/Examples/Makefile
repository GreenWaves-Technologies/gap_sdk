BUILD_DIR = BUILD_MODEL
APP = TestGap
APP_SRCS =  TestMat.c $(BUILD_DIR)/Kernels.c $(TILER_GENERATOR_PATH)/MatMult/MatMultBasicKernels.c

APP_CFLAGS = -O3 -g #-DWITH_FLOAT16

APP_CFLAGS += -I$(AT_HOME)/Emulation -I$(AT_HOME)/Autotiler -I$(BUILD_DIR) -I$(TILER_GENERATOR_PATH)/MatMult

APP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
APP_LDFLAGS +=  -flto -Wl,--gc-sections

include $(RULES_DIR)/pmsis_rules.mk
