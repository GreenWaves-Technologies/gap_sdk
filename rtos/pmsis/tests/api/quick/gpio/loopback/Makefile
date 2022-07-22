APP = test
APP_SRCS += test.c $(CURDIR)/../../../common/testbench.c
APP_CFLAGS += -Os -g -I$(CURDIR)/../../../common
APP_LDFLAGS += -Os -g

export GVSOC_TESTBENCH=1

ifeq ($(GAP9_WLCSP), 1)
PULP_CFLAGS += -DGAP9_WLCSP
endif

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

include $(RULES_DIR)/pmsis_rules.mk
