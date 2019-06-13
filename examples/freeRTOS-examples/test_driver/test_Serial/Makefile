# User Test
#------------------------------------
APP_SRC 	= ./main_Serial.c
DEMO_SRC	+=
INC_PATH	+=

ifeq ($(PRINTF_UART), 1)
FREERTOS_FLAGS  += -DPRINTF_UART
endif

include $(GAP_SDK_HOME)/tools/rules/freeRTOS_rules.mk
