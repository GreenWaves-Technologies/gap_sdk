# User Test
#------------------------------------

APP              = test_aes_dual_core_hyper_multiple_hyper
APP_SRCS        += test_aes_dual_core_hyper_multiple_hyper.c
APP_INC         +=
APP_CFLAGS      +=
#APP_CFLAGS      += -DTRACE_HYPERBUS=1

# Since this test uses freertos internals, we need to force freertos
override PMSIS_OS=freertos
CONFIG_HYPERRAM=1

#set VIP CUSTOM MODE
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
#set Hyper0_CS1 (code)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
#set Hyper0_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
#set Hyper1_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_HYPERRAM_VIP=1

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
