# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test.c
# App includes
APP_INC	         =
# Compiler flags
APP_CFLAGS       = -O2 -g
# Linker flags
APP_LDFLAGS      =

#APP_CFLAGS += -DTRACE_CPI
#APP_CFLAGS += -DOUT_TO_DISPLAY

#Choose between RGB (565) and Grayscale
#APP_CFLAGS += -DGRAYSCALE

# Include GAP_LIB/IMGIO
CONFIG_GAP_LIB_IMGIO = 1

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk
