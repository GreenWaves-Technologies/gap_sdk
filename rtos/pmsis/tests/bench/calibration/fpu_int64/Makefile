APP           = test
APP_SRCS      = test.c
APP_ASM_SRCS  = test_asm.S
APP_CFLAGS    = -O3 -g

NB_PE ?= 8

APP_ARCH_CFLAGS ?=  -march=rv32imcxgap9 -mPE=$(NB_PE) -mFC=1
APP_ARCH_LDFLAGS ?=  -march=rv32imcxgap9 -mPE=$(NB_PE) -mFC=1

APP_CFLAGS += -DNB_PE=$(NB_PE)

ifdef VERBOSE
APP_CFLAGS += -DVERBOSE=$(VERBOSE)
endif

include $(RULES_DIR)/pmsis_rules.mk
