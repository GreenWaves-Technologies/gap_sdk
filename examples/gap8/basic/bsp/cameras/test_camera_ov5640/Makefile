# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test.c ov5640_config.c
# App includes
APP_INC	         =
# Compiler flags
APP_CFLAGS       = -O3 -g
# Linker flags
APP_LDFLAGS      =

#APP_CFLAGS += -DHIMAX
APP_CFLAGS += -DDEBUG
#APP_CFLAGS += -DJPEG

# Include GAP_LIB/IMGIO
CONFIG_GAP_LIB_IMGIO = 1

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true

clean::
	rm -rf output*

include $(RULES_DIR)/pmsis_rules.mk
