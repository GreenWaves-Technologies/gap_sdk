APP = test
APP_SRCS += test.c
APP_CFLAGS += -O2 -g
APP_LDFLAGS += -lgaplib


#APP_CFLAGS += -DTRACE_CPI
#APP_CFLAGS += -DOUT_TO_DISPLAY

## This is not fully implemented
#APP_CFLAGS += -DSTORE_IMG_TO_L3

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk