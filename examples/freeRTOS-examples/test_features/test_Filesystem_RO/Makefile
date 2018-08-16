# User Test
#------------------------------------
APP_SRC 	= ./main_Filesystem.c
DEMO_SRC	+=
INC_PATH	+=

ifeq ($(WRITE), true)
PLPBRIDGE_FLAGS	+= -f $(FILE)
endif

ifeq ($(HYPER), true)
PLPBRIDGE_FLAGS	+= -hyper
else
PLPBRIDGE_FLAGS	+= -jtag
endif

include $(GAP_SDK_HOME)/tools/rules/freeRTOS_rules.mk
