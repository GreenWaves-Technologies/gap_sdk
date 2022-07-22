# User Test
#------------------------------------

APP              = aes_dual_core_simple
APP_SRCS        += aes_dual_core_simple.c
APP_INC         +=
APP_CFLAGS      +=

# Since this test uses freertos internals, we need to force freertos
override PMSIS_OS=freertos

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
