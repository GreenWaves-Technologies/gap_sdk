# User Test
#------------------------------------

APP              = test
APP_SRCS        += i2c_scan.c
APP_INC         +=
APP_CFLAGS      +=


ifeq '$(platform)' 'gvsoc'
# Overwrite the default target so that GVSOC simulates our board
# First name is the class name, second one is the python module
export GAPY_PY_TARGET=My_board@my_board
endif

# Append current directory to python path so that it finds our board and module
export PYTHONPATH:=$(CURDIR):$(PYTHONPATH)

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
