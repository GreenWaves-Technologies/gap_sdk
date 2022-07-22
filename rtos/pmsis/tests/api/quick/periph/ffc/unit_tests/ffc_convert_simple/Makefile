# User Test
#------------------------------------

APP              = ffc_convert_simple
APP_SRCS        += ffc_convert_simple.c ../ffc_utils/ffc_test_utils.c
APP_INC         += ../ffc_utils
APP_CFLAGS      +=

PMSIS_OS=freertos

ifdef TEST_SIZE
APP_CFLAGS += -DTEST_SIZE=$(TEST_SIZE)
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
