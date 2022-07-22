APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

CONFIG_VIRTUAL_EEPROM = 1
config_args += --config-opt=rtl/args=+ENABLE_VIP_I2C_LOOPBACK_1=1
config_args += --config-opt=rtl/args=+ENABLE_VIP_I2C_LOOPBACK_2=1



include $(RULES_DIR)/pmsis_rules.mk
