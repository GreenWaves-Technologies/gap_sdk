APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

NB_PE ?= 8

CONFIG_TESTBENCH=1
CONFIG_SLOW_OSC=1

APP_ARCH_CFLAGS ?=  -march=rv32imcxgap9 -mPE=$(NB_PE) -mFC=1
APP_ARCH_LDFLAGS ?=  -march=rv32imcxgap9 -mPE=$(NB_PE) -mFC=1

APP_CFLAGS += -DNB_PE=$(NB_PE)

RAM_TYPE ?= HYPERRAM
ifeq '$(RAM_TYPE)' 'HYPERRAM'
APP_CFLAGS += -DUSE_HYPERRAM
endif

include $(RULES_DIR)/pmsis_rules.mk