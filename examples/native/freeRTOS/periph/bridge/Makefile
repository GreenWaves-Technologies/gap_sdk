# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_bridge.c
APP_INC         +=
APP_CFLAGS      +=

PMSIS_OS = freertos
BOARD_NAME ?= gapuino

PLPBRIDGE_FLAGS += -fileIO 2

clean::
	rm -rf out_bridge.c

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
