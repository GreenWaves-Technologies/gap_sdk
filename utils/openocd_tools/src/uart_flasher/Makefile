# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_uart_input.c
APP_INC         +=
APP_CFLAGS      += #-DUART_FLOW_CONTROL_EMU

FLASH_IMAGE_SIZE ?= 1048510   # Just a default value for test

APP_CFLAGS += -DIMG_SIZE=$(FLASH_IMAGE_SIZE)

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
