-include $(RULES_DIR)/properties.mk

RUNNER_CONFIG = $(CURDIR)/config-$(TARGET_NAME).ini

ifeq (,$(wildcard $(RUNNER_CONFIG)))
RUNNER_CONFIG = $(CURDIR)/config.ini
endif

USE_PMSIS_BSP=1

PLPBRIDGE_FLAGS += -f flash_file_0.bin flash_file_1.bin -jtag

override runner_args += --config-opt=flash/fs/files=$(CURDIR)/flash_file_0.bin --config-opt=flash/fs/files=$(CURDIR)/flash_file_1.bin



APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

ifdef USE_CLUSTER
APP_CFLAGS += -DUSE_CLUSTER
endif


include $(RULES_DIR)/pmsis_rules.mk
