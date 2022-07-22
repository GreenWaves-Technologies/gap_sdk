APP = test
APP_SRCS = test.c

TEST_DURATION ?= 50

APP_CFLAGS += -O3 -g
APP_CFLAGS += -DTEST_DURATION=$(TEST_DURATION)

ifdef WITH_IRQ
APP_CFLAGS += -DWITH_IRQ=1
endif

include $(RULES_DIR)/pmsis_rules.mk
