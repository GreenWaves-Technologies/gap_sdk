# User Test
#------------------------------------

APP              = test
APP_SRCS         = test.c
APP_INC          =
APP_CFLAGS       =

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

UART_ITF ?= 0
UART_BAUDRATE ?= 115200
# Timeout mode:
# 0 - SW trigger
# 1 - Transfer
# 2 - RXTX
UART_TIMEOUT_MODE ?= 1

ifeq ($(UART_TIMEOUT_MODE), 0)
APP_SRCS         = test_sw.c
endif				#UART_TIMEOUT_MODE

APP_CFLAGS      += -DUART_ITF=$(UART_ITF) -DUART_BAUDRATE=$(UART_BAUDRATE)
APP_CFLAGS      += -DUART_TIMEOUT_MODE=$(UART_TIMEOUT_MODE)

override PMSIS_OS=freertos

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
