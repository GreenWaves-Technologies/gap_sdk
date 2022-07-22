# User Test
#------------------------------------

APP              = ffc_convert_stressed
APP_SRCS        += ffc_convert_stressed.c ../../unit_tests/ffc_utils/ffc_test_utils.c
APP_INC         += ../../unit_tests/ffc_utils
APP_CFLAGS      +=

PMSIS_OS=freertos

CONFIG_TESTBENCH=1

# Hyper custom configuration
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_HYPERRAM_VIP=1

ifdef TEST_SIZE
APP_CFLAGS += -DTEST_SIZE=$(TEST_SIZE)
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
