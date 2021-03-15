# User Test
#------------------------------------

APP              = test
APP_SRCS        += i2c_eeprom_pulp_fmc.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(platform), rtl)
config_args += --config-opt=rtl/args=+ENABLE_VIP_EEPROM_0=1
endif

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
