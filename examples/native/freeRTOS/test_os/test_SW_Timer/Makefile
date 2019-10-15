# User Test
#------------------------------------

APP              = test
APP_SRCS        += main_SW_Timer.c
APP_INC         +=
APP_CFLAGS      += -DconfigUSE_TIMERS=1

PMSIS_OS = freertos
BOARD_NAME ?= gapuino

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
