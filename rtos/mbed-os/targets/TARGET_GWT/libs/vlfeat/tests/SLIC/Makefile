LIB_C           = ../../src/hog.parallel.dyn.c \
                  ../../src/generic_hog.c      \
                  ../../src/slic.c             \

TEST_C          = $(LIB_C) test_slic.c

INC            += ../../inc
MBED_FLAGS      = -fopenmp

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

# ar -rcs libgomp.a BUILD/GAP8/GCC_RISCV/src/root.o
# -lgomp
# include src/config/pulp
