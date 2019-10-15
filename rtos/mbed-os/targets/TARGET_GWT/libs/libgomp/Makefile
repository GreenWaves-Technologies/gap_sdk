TEST_C          = ./src/root.c src/test.c
MBED_FLAGS      = -DOMP_NOWAIT_SUPPORT -fopenmp
INC             += ./src/config/pulp

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

# ar -rcs libgomp.a BUILD/GAP8/GCC_RISCV/src/root.o
# -lgomp
# include src/config/pulp
