# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_camera_lcd.c
APP_INC         += .
APP_CFLAGS      += -O3 -g

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif

ifeq ($(BOARD_NAME), gapuino)
APP_CFLAGS      += -DHIMAX
endif

ifeq ($(BRIDGE), 1)
APP_CFLAGS += -DUSE_BRIDGE
PLPBRIDGE_FLAGS += -fileIO 2
endif

override runner_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override runner_args += --config-opt=display/enabled=true

RUNNER_CONFIG = $(CURDIR)/config.ini

include $(RULES_DIR)/pmsis_rules.mk
