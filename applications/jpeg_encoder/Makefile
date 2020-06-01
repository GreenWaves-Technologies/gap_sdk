# Only Pulp-Os for now.
PMSIS_OS=pulpos

APP = test
APP_SRCS += test.c ImgIO.c
APP_CFLAGS += -O3 -g -Werror
APP_LDFLAGS += -g -lgaplib

IMAGE1=imgTest0.pgm

export GAP_USE_OPENOCD=1

PLPBRIDGE_FLAGS += -f $(IMAGE1)
override config_args += --config-opt=flash/fs/files=$(CURDIR)/$(IMAGE1)

include $(RULES_DIR)/pmsis_rules.mk
