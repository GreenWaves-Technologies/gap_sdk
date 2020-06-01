APP = test
APP_SRCS += test.c
APP_CFLAGS += -O3 -g
APP_LDFLAGS += -lgaplib

#APP_CFLAGS += -DHIMAX

#APP_CFLAGS += -DTRACE_CPI

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk
