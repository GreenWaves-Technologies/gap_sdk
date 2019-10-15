-include $(RULES_DIR)/properties.mk

RUNNER_CONFIG = $(CURDIR)/config-$(TARGET_NAME).ini

ifeq (,$(wildcard $(RUNNER_CONFIG)))
RUNNER_CONFIG = $(CURDIR)/config.ini
endif

USE_PMSIS_BSP=1

ifdef USE_CLUSTER
APP_CFLAGS += -DUSE_CLUSTER
endif

ifdef READ
APP_CFLAGS += -DTEST_READ
endif

ifdef WRITE
APP_CFLAGS += -DTEST_WRITE
endif

ifdef QUICK
APP_CFLAGS += -DTEST_QUICK
endif

ifdef BASIC
APP_CFLAGS += -DTEST_BASIC
endif

ifdef ROBUST
APP_CFLAGS += -DTEST_ROBUST
endif

ifdef ASYNC
APP_CFLAGS += -DTEST_ASYNC
endif

ifdef SYNC
APP_CFLAGS += -DTEST_SYNC
endif

ifdef ASYNC_2D
APP_CFLAGS += -DTEST_ASYNC_2D
endif

ifdef SYNC_2D
APP_CFLAGS += -DTEST_SYNC_2D
endif

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

include $(RULES_DIR)/pmsis_rules.mk
