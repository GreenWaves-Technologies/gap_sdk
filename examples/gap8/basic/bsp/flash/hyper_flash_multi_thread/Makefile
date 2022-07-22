# User Test
#------------------------------------

APP              = test
APP_SRCS        += hyper_flash_multi_thread.c
APP_INC         +=
APP_CFLAGS      +=

# Only for FreeRTOS
PMSIS_OS = freertos

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
