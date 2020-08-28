# User Test
#------------------------------------

APP              = test
APP_SRCS        += i2c_slave_loopback.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif #ASYNC

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
