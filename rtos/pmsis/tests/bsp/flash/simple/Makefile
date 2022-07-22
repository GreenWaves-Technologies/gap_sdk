-include $(RULES_DIR)/properties.mk

ifdef FLASH_TYPE
ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'
APP_CFLAGS += -DUSE_HYPERFLASH
else
ifeq '$(FLASH_TYPE)' 'MRAM'
APP_CFLAGS += -DUSE_MRAM
CONFIG_MRAM=1
else
APP_CFLAGS += -DUSE_SPIFLASH
endif
endif
APP_CFLAGS += -DFLASH_TYPE=$(FLASH_TYPE)
endif

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

TEST_TYPE ?= quick

ifeq '$(TEST_TYPE)' 'fast'
APP_CFLAGS += -DTEST_TYPE_FAST
endif

ifeq '$(TEST_TYPE)' 'quick'
APP_CFLAGS += -DTEST_TYPE_QUICK
endif

ifeq '$(TEST_TYPE)' 'long'
APP_CFLAGS += -DTEST_TYPE_LONG
endif

ifeq '$(TEST_TYPE)' 'longlong'
APP_CFLAGS += -DTEST_TYPE_LONGLONG
endif

include $(RULES_DIR)/pmsis_rules.mk
