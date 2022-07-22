# User Test
#------------------------------------

APP              = test_aes_dual_core_hyper
APP_SRCS        += test_aes_dual_core_hyper.c
APP_INC         +=
APP_CFLAGS      +=

# Since this test uses freertos internals, we need to force freertos
override PMSIS_OS=freertos
CONFIG_HYPERRAM=1

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
