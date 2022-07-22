# User Test
#------------------------------------

APP              = i2c_master_slave
APP_SRCS        += i2c_master_slave.c
APP_INC         +=
APP_CFLAGS      +=

ifeq '$(platform)' 'rtl'
# enable I2C Bus
config_args += --config-opt=rtl/args=+ENABLE_VIP_I2C0_BUS0=1
config_args += --config-opt=rtl/args=+ENABLE_VIP_I2C1_BUS0=1
config_args += --config-opt=rtl/args=+ENABLE_VIP_I2C2_BUS0=1
config_args += --config-opt=rtl/args=+ENABLE_VIP_I2C3_BUS0=1

ifdef INTERFACE_MASTER
APP_CFLAGS += -DINTERFACE_MASTER=$(INTERFACE_MASTER)
endif

ifdef INTERFACE_SLAVE
APP_CFLAGS += -DINTERFACE_SLAVE=$(INTERFACE_SLAVE)
endif

endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
