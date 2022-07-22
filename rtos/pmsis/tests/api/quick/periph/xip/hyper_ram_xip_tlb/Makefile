# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_hyper_ram.c
APP_INC         +=
APP_CFLAGS      +=

override PMSIS_OS = freertos

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif


#set VIP CUSTOM MODE
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
#set Hyper0_CS1 (code)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
#set Hyper0_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
#set Hyper1_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_HYPERRAM_VIP=1

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
