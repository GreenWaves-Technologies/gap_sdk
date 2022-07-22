# User Test
#------------------------------------

APP              = test
APP_SRCS        += flash_dumper.c
APP_INC         +=
APP_CFLAGS      +=

# Provide the flash image size
FLASH_IMAGE_SIZE ?= 61472 

APP_CFLAGS += -DTOTAL_SIZE=$(FLASH_IMAGE_SIZE)

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
