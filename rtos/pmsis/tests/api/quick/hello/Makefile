APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

ifdef CLUSTER
APP_CFLAGS += -DCLUSTER=1
endif

ifdef ENABLE_MAX_FAST_CLOCK
override config_args += --config-opt=**/rtl/args=+ENABLE_MAX_FAST_CLOCK=1
override config_args += --config-opt=**/rtl/args=+DISABLE_FAST_CLOCK=1
endif

include $(RULES_DIR)/pmsis_rules.mk
