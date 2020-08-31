# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_hyper_ram_multi_thread.c
APP_INC         +=
APP_CFLAGS      +=

# Only for FreeRTOS
PMSIS_OS = freertos

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
