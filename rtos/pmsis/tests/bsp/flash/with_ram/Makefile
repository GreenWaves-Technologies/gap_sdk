-include $(RULES_DIR)/properties.mk

FLASH_TYPE ?= HYPER_FLASH

ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'

APP_CFLAGS += -DUSE_HYPERFLASH

RUNNER_CONFIG = $(CURDIR)/config-$(TARGET_NAME).ini

ifeq (,$(wildcard $(RUNNER_CONFIG)))
RUNNER_CONFIG = $(CURDIR)/config.ini
endif

else

APP_CFLAGS += -DUSE_SPIFLASH
RUNNER_CONFIG = $(CURDIR)/config-spi.ini
CONFIG_ATXP032 = 1
CONFIG_SPIRAM = 1

endif

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g -DFLASH_TYPE=$(FLASH_TYPE)


include $(RULES_DIR)/pmsis_rules.mk
