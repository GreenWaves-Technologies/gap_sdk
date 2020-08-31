PULP_APP = MFCC_Gap
PULP_APP_SRCS = MFCC_Gap.c ../FFT_Lib/FFT_Lib.c ../FFT_Lib/TwiddlesDef.c ../FFT_Lib/SwapTablesDef.c

PULP_ARCH_CFLAGS=-march=IMXgap8 -mPE=8 -mFC=1
PULP_ARCH_LDFLAGS=-march=IMXgap8 -mPE=8 -mFC=1

PULP_CFLAGS += -O3 -DFFTR2_512 -DOLD_RUNTIME
PULP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections
PULP_LDFLAGS +=  -flto -Wl,--gc-sections

include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk

