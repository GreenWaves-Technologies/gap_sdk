# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_hyper_ram_flash.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
