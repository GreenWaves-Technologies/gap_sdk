# User Test
#------------------------------------

APP              = i2c_master
APP_SRCS        += i2c_master.c
APP_INC         +=
APP_CFLAGS      += -Os

# CONFIG_VIRTUAL_EEPROM=1

ifeq '$(platform)' 'rtl'
# enable I2C Bus
#config_args += --config-opt=rtl/args=+ENABLE_VIP_I2C0_BUS0=1

endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
