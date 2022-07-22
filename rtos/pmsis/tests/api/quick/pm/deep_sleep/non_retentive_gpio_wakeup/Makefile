APP = test
APP_SRCS += test.c $(CURDIR)/../../../../common/testbench.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

override config_args += --config-opt=**/runner/jtag_mode=disable

CONFIG_SLOW_OSC=1

FLASH_TYPE ?= HYPER_FLASH


ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'

APP_CFLAGS += -DUSE_HYPERFLASH

else

ifeq '$(FLASH_TYPE)' 'MRAM'

APP_CFLAGS += -DUSE_MRAM

else

APP_CFLAGS += -DUSE_SPIFLASH
RUNNER_CONFIG = $(CURDIR)/config-spi.ini
CONFIG_ATXP032 = 1

endif

endif

TESTBENCH_UART_ID ?= 1
TESTBENCH_UART_BAUDRATE ?= 1000000

APP_CFLAGS += -DTESTBENCH_UART_ID=$(TESTBENCH_UART_ID) -DTESTBENCH_UART_BAUDRATE=$(TESTBENCH_UART_BAUDRATE) -I$(CURDIR)/../../../../common

export GVSOC_TESTBENCH=1

override config_args += --config-opt=**/runner/jtag_mode=disable
override config_args += --config-opt=**/testbench/testbench/uart_id=$(TESTBENCH_UART_ID)
override config_args += --config-opt=**/testbench/testbench/uart_baudrate=$(TESTBENCH_UART_BAUDRATE)
override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true


include $(RULES_DIR)/pmsis_rules.mk