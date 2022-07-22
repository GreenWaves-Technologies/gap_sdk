APP = test
APP_SRCS = test.c
APP_CFLAGS = -O3 -g

CONFIG_UART = 1

RUNNER_CONFIG = $(CURDIR)/config.ini

override config_args += --config-opt=**/runner/peripherals=true
override config_args += --config-opt=**/board/uart/loopback=true
override config_args += --config-opt=**/board/uart/stdout=false

include $(RULES_DIR)/pmsis_rules.mk
