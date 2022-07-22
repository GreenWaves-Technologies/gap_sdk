APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

override config_args += --config-opt=**/runner/jtag_mode=disable

CONFIG_SLOW_OSC=1


export CONFIG_TESTBENCH=1


include $(RULES_DIR)/pmsis_rules.mk
