# User Test
#------------------------------------

APP              = test
APP_SRCS        += test.c
APP_CFLAGS      += -O3 -g
APP_LDFLAGS 	+= -lgaplib

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif


APP_CFLAGS += -DUSE_BRIDGE
PLPBRIDGE_FLAGS += -fileIO

#override runner_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
#override runner_args += --config-opt=display/enabled=true

#RUNNER_CONFIG = $(CURDIR)/config.ini

clean::
	rm -rf *.pgm

include $(RULES_DIR)/pmsis_rules.mk
