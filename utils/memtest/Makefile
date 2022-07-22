APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

ifdef PRIV_TEST
	APP_CFLAGS += -DPRIV_TEST=1
	APP_LINK_SCRIPT= ./GAP9-l2-private-test.ld
else
	APP_LINK_SCRIPT= ./GAP9-l2-shared-test.ld
endif

ifdef CLUSTER_TEST
	APP_CFLAGS += -DCLUSTER_TEST=1
endif

PMSIS_OS=freertos


include $(RULES_DIR)/pmsis_rules.mk
