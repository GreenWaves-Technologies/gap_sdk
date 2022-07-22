# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test_camera_lcd.c
# App includes
APP_INC	         = .
# Compiler flags
APP_CFLAGS       = -O3 -g
# Linker flags
APP_LDFLAGS      =

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif

ifeq ($(BOARD_NAME), gapuino)
APP_CFLAGS      += -DHIMAX
endif

# Include GAP_LIB/IMGIO
CONFIG_GAP_LIB_IMGIO = 1

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=display/enabled=true

clean::
	@rm -rf img_OUT_*.ppm

include $(RULES_DIR)/pmsis_rules.mk
