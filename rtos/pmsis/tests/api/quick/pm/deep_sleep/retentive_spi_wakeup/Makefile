APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

override config_args += --config-opt=**/runner/jtag_mode=disable
# Pull down SPI2 wakeup pads SCK and SDI to avoid 'X' propagation in netlist
override config_args += --config-opt=**/rtl/args=+EXEC_TEST=PULL_DOWN_SPI2_WAKEUP_PADS

TEST_DURATION ?= 50
CONFIG_SLOW_OSC=1

APP_CFLAGS += -DTEST_DURATION=$(TEST_DURATION)

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

export CONFIG_TESTBENCH=1

include $(RULES_DIR)/pmsis_rules.mk