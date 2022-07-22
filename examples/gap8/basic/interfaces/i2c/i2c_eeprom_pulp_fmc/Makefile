# User Test
#------------------------------------

APP              = test
APP_SRCS        += i2c_eeprom_pulp_fmc.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(platform), rtl)
config_args += --config-opt=rtl/args=+ENABLE_VIP_EEPROM_0=1
endif

#runner_args =--trace=board.*i2c:.log --trace-level=trace
#runner_args =--trace=board.*i2c --trace-level=trace
#runner_args =--trace=eeprom --trace-level=trace
#runner_args =--trace=corruptor --trace-level=trace
#runner_args =--trace=corruptor --trace=i2c_bus --trace-level=trace
# Overwrite the default target so that GVSOC simulates our board
# First name is the class name, second one is the python module
export GAPY_PY_TARGET=My_board@my_board

# Append current directory to python path so that it finds our board and module
export PYTHONPATH:=$(CURDIR):$(PYTHONPATH)


include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
