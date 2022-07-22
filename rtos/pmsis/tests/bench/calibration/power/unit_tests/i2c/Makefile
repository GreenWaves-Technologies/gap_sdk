APP = test
APP_SRCS += test.c

CONFIG_I2C = 1

APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

APP_INC            += $(GAP_LIB_PATH)/testbench
APP_SRCS           += $(GAP_LIB_PATH)/testbench/testbench.c $(GAP_LIB_PATH)/testbench/testlib.c \
	$(GAP_LIB_PATH)/testbench/testlib_hyper.c $(GAP_LIB_PATH)/testbench/testlib_i2s.c \
	$(GAP_LIB_PATH)/testbench/testlib_uart.c $(GAP_LIB_PATH)/testbench/testlib_i2c.c

config_args += --config-opt=rtl/args=+ENABLE_VIP_EEPROM_0=1

override config_args += --config-opt=**/rtl/args=+CONFIG_UART_BAUDRATE=1000000
override config_args += --config-opt=**/rtl/args=+CONFIG_VIP_UART_TX_EN=1

include $(RULES_DIR)/pmsis_rules.mk
