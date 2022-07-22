APP = test
APP_SRCS += test.c $(CURDIR)/../../../../common/testbench.c
APP_CFLAGS += -Os -g -I$(CURDIR)/../../../../common
APP_LDFLAGS += -Os -g

prescaler ?= 50 
APP_CFLAGS += -DSOC_CLK
APP_CFLAGS += -DTS_PRESCALER=$(prescaler)

test ?= 5

ifeq ($(GAP9_WLCSP), 1)
PULP_CFLAGS += -DGAP9_WLCSP
endif

ifneq ($(test), )
APP_CFLAGS += -DTH_TEST=$(test)
endif

export GVSOC_TESTBENCH=1

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

ifeq ($(gui), 1)
override runner_args=--gui
endif

include $(RULES_DIR)/pmsis_rules.mk
