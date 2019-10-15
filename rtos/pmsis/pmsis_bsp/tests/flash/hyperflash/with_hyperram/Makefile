-include $(RULES_DIR)/properties.mk

FLASH_TYPE ?= HYPER_FLASH

ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'

RUNNER_CONFIG = $(CURDIR)/config-$(TARGET_NAME).ini

ifeq (,$(wildcard $(RUNNER_CONFIG)))
RUNNER_CONFIG = $(CURDIR)/config.ini
endif

endif

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g -DFLASH_TYPE=$(FLASH_TYPE)
USE_PMSIS_BSP=1


include $(RULES_DIR)/pmsis_rules.mk
