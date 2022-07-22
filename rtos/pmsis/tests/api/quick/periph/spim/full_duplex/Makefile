-include $(RULES_DIR)/properties.mk

CONFIG_SPIM=1

APP_CFLAGS += $(TEST_CFLAGS)

APP_SRCS += $(CURDIR)/../../../../common/testbench.c

TEST_DURATION ?= 50
APP_CFLAGS += -DTEST_DURATION=$(TEST_DURATION)

TESTBENCH_UART_ID ?= 1
TESTBENCH_UART_BAUDRATE ?= 1000000

APP_CFLAGS += -DTESTBENCH_UART_ID=$(TESTBENCH_UART_ID) -DTESTBENCH_UART_BAUDRATE=$(TESTBENCH_UART_BAUDRATE) -I$(CURDIR)/../../../../common

APP_CFLAGS += -DSPIM_ITF=0
APP_CFLAGS += -DSPIM_CS=0
export GVSOC_TESTBENCH=1
override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true
override config_args += --config-opt=**/testbench/testbench/uart_id=$(TESTBENCH_UART_ID)
override config_args += --config-opt=**/testbench/testbench/uart_baudrate=$(TESTBENCH_UART_BAUDRATE)

APP = test
APP_SRCS += test.c
APP_CFLAGS += -O3 -g

include $(RULES_DIR)/pmsis_rules.mk
