# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_hyper_ram.c
APP_INC         +=
APP_CFLAGS      += -Wno-shift-count-overflow

override PMSIS_OS = freertos

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

ifeq ($(XIP_ACCESS_TEST), 1)
APP_CFLAGS      += -DXIP_ACCESS_TEST=1
endif

ifeq ($(XIP_RECONF), 1)
APP_CFLAGS      += -DXIP_RECONF=1
endif

ifeq ($(ILLEGAL_INSN_ACCESS), 1)
APP_CFLAGS      += -DILLEGAL_INSN_ACCESS=1
endif

ifeq ($(ILLEGAL_DATA_ACCESS), 1)
APP_CFLAGS      += -DILLEGAL_DATA_ACCESS=1
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
