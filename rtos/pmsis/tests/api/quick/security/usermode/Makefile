APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

override PMSIS_OS=freertos

#CLUSTER=1

ifdef CLUSTER
APP_CFLAGS += -DCLUSTER=1
endif

include $(RULES_DIR)/pmsis_rules.mk
