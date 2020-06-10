APP = test
APP_SRCS += test.c
APP_CFLAGS += -O2 -g
APP_LDFLAGS += -lgaplib

APP_CFLAGS += -DOUT_TO_DISPLAY
#APP_CFLAGS += -DTRACE_CPI

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk