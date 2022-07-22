# User Test
#------------------------------------

APP              = test_aes_dual_core_hyper_stress
APP_SRCS        += test_aes_dual_core_hyper_stress.c
APP_INC         +=
APP_CFLAGS      += -Os

# Since this test uses freertos internals, we need to force freertos
override PMSIS_OS=freertos
CONFIG_HYPERRAM=1

TEST_DURATION ?= 50
APP_CFLAGS += -DTEST_DURATION=$(TEST_DURATION)

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
