# User Test
#------------------------------------

APP              = test_aes_dual_core_hyper_multiple_confs
APP_SRCS        += test_aes_dual_core_hyper_multiple_confs.c
APP_INC         +=
APP_CFLAGS      +=
#APP_CFLAGS      += -DTRACE_HYPERBUS=1

# Since this test uses freertos internals, we need to force freertos
override PMSIS_OS=freertos
CONFIG_HYPERRAM=1

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
