APP = test
APP_SRCS += test.c $(CURDIR)/../../../../../common/testbench.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

TESTBENCH_UART_ID ?= 1
TESTBENCH_UART_BAUDRATE ?= 1000000

CONFIG_SLOW_OSC=1

APP_CFLAGS += -DTEST_DURATION=$(TEST_DURATION)

APP_CFLAGS += -DTESTBENCH_UART_ID=$(TESTBENCH_UART_ID) -DTESTBENCH_UART_BAUDRATE=$(TESTBENCH_UART_BAUDRATE) -I$(CURDIR)/../../../../../common

ifdef LONG_PERIOD
APP_CFLAGS += -DLONG_PERIOD=1
endif


export GVSOC_TESTBENCH=1

override config_args += --config-opt=**/runner/jtag_mode=disable
override config_args += --config-opt=**/testbench/testbench/uart_id=$(TESTBENCH_UART_ID)
override config_args += --config-opt=**/testbench/testbench/uart_baudrate=$(TESTBENCH_UART_BAUDRATE)
override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

include $(RULES_DIR)/pmsis_rules.mk
