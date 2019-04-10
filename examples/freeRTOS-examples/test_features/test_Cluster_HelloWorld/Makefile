# User Test
#------------------------------------
APP_SRC 	= ./main_HelloWorld.c
DEMO_SRC	+=
INC_PATH	+=

ifeq ($(NO_OS), true)
FREERTOS_FLAGS += -DNO_SCHEDULER=1
FREERTOS_FLAGS += -DPRINTF_UART=1
endif

include $(GAP_SDK_HOME)/tools/rules/freeRTOS_rules.mk
