# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test.c
# App includes
APP_INC	         =
# Compiler flags
APP_CFLAGS       = -O3 -g
# Linker flags
APP_LDFLAGS      =

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif

APP_CFLAGS += -DUSE_BRIDGE
PLPBRIDGE_FLAGS += -fileIO

# Include GAP_LIB/IMGIO
CONFIG_GAP_LIB_IMGIO = 1

#override runner_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
#override runner_args += --config-opt=display/enabled=true

#RUNNER_CONFIG = $(CURDIR)/config.ini

clean::
	rm -rf *.pgm

include $(RULES_DIR)/pmsis_rules.mk
