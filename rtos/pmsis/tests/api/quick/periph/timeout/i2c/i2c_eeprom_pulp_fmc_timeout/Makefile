# User Test
#------------------------------------

APP              = test
APP_SRCS         = test.c
APP_INC          =
APP_CFLAGS       =

ifeq ($(platform), rtl)
config_args += --config-opt=rtl/args=+ENABLE_VIP_EEPROM_0=1
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
