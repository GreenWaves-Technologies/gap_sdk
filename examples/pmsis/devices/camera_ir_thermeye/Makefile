# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_thermeye.c ImgIO.c
APP_INC	        += .
APP_CFLAGS      +=
APP_LDFLAGS     +=

BOARD_NAME = gapoc_b

BRIDGE = 1

ifeq ($(BRIDGE), 1)
APP_CFLAGS += -DUSE_BRIDGE
PLPBRIDGE_FLAGS += -fileIO 2
endif

include $(RULES_DIR)/pmsis_rules.mk
