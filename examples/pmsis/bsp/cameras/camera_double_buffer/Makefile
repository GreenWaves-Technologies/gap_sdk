# User Test
#------------------------------------

APP              = test
APP_SRCS        += camera_soft_double_buffer.c
APP_INC         +=
APP_CFLAGS      += -O3 -g

ifeq ($(BOARD_NAME), gap9_v2)
APP_CFLAGS      += -DHIMAX
endif

ifeq ($(BOARD_NAME), gapuino)
APP_CFLAGS      += -DHIMAX
endif

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm

include $(RULES_DIR)/pmsis_rules.mk
