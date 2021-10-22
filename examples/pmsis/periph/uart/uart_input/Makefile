# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_uart_input.c
APP_INC         +=
APP_CFLAGS      += #-DUART_FLOW_CONTROL_EMU

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
