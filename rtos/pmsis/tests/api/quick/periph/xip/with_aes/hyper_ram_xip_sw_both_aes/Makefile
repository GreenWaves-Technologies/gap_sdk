# User Test
#------------------------------------

APP              = test_hyper_ram_xip_sw_both_aes
APP_SRCS        += test_hyper_ram_xip_sw_both_aes.c
APP_INC         +=
APP_CFLAGS      +=

override PMSIS_OS = freertos

# ==================
# CUSTOM Hyper Setup
# ==================

#set VIP CUSTOM MODE
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
#set Hyper0_CS1 (code)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
#set Hyper0_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
#set Hyper1_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_HYPERRAM_VIP=1

ifeq ($(GAP9_WLCSP), 1)
APP_CFLAGS += -DGAP9_WLCSP
endif

TEST_DURATION ?= 50

APP_CFLAGS += -DTEST_DURATION=$(TEST_DURATION)

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
