# User Test
#------------------------------------

APP              = test
APP_SRCS        += i2c_scan.c
APP_INC         +=
APP_CFLAGS      +=


runner_args =--trace=corruptor --trace-level=trace
#runner_args =--trace=board.*i2c --trace-level=trace
#runner_args =--trace=board.*i2c:gvsoc.log --trace-level=trace
#runner_args =--trace=eeprom
#runner_args +=--trace-level=trace
# Overwrite the default target so that GVSOC simulates our board
# First name is the class name, second one is the python module
export GAPY_PY_TARGET=My_board@my_board

# Append current directory to python path so that it finds our board and module
export PYTHONPATH:=$(CURDIR):$(PYTHONPATH)

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
