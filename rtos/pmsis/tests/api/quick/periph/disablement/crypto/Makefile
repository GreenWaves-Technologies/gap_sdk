APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

PMSIS_OS=freertos

ifeq ($(TEST_QK), 1)
APP_CFLAGS += -DTEST_QK=1
endif

ifeq ($(AES), 1)
APP_CFLAGS += -DAES=1
endif

ifeq ($(AES), 2)
APP_CFLAGS += -DAES=2
endif

include $(RULES_DIR)/pmsis_rules.mk
