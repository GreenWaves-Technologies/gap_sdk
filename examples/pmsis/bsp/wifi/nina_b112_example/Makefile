APP = ble
APP_SRCS += ble_main.c ImgIO.c
APP_CFLAGS += -O3 -g
USE_PMSIS_BSP=1

BOARD_NAME = gapoc_a

ifeq '$(BOARD_NAME)' 'gapuino'
APP_CFLAGS = -DGAPUINO -DHIMAX
endif

ifeq '$(BOARD_NAME)' 'gapoc_a'
PLPBRIDGE_FLAGS += -ftdi -fileIO 10
APP_CFLAGS = -DGAPOC_A
endif

include $(RULES_DIR)/pmsis_rules.mk
