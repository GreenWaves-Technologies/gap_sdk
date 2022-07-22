APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

ifeq ($(TEST_START), 1)
APP_CFLAGS += -DQUIDDIKEY_TEST_START=1
endif

override PMSIS_OS = freertos

include $(RULES_DIR)/pmsis_rules.mk
