APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

TEST_DURATION ?= 100	
APP_CFLAGS += -DTEST_DURATION=$(TEST_DURATION)

include $(RULES_DIR)/pmsis_rules.mk
