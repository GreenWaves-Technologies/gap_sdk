# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_thermal_eye.c ImgIO.c
APP_INC	        += .
APP_CFLAGS      +=

PMSIS_OS ?= freertos
BOARD_NAME = gapoc_b

BRIDGE = 1

ifeq ($(BRIDGE), 1)
APP_CFLAGS += -DUSE_BRIDGE
PLPBRIDGE_FLAGS += -fileIO 4
endif

include $(RULES_DIR)/pmsis_rules.mk
