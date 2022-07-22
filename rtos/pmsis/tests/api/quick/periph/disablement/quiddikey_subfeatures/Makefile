APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

PMSIS_OS=freertos

ifeq ($(TEST_START), 1)
APP_CFLAGS += -DQUIDDIKEY_TEST_START=1
endif

include $(RULES_DIR)/pmsis_rules.mk
