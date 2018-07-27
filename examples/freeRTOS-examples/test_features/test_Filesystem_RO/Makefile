# User Test
#------------------------------------
APP_SRC 	= ./main_Filesystem.c
DEMO_SRC	+=
INC_PATH	+=

# WRITE		= $(FLASH)
# DEBUG		= $(NDEBUG)
FILE		= hello.txt

ifeq ($(WRITE), true)
PLPBRIDGE_FLAGS	+= -f $(FILE) -jtag
endif

ifeq ($(DEBUG), true)
FREERTOS_FLAGS	+= -DNDEBUG
endif

include $(GAP_SDK_HOME)/tools/rules/freeRTOS_rules.mk
