# User Test
#------------------------------------

APP              = test_hyper_ram_xip_aes
APP_SRCS        += test_hyper_ram_xip_aes.c
APP_INC         +=
APP_CFLAGS      += -Wno-shift-count-overflow

override PMSIS_OS = freertos

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
