# User Test
#------------------------------------

APP              = test
APP_SRCS         = test_dmacpy.c
APP_INC          =
APP_CFLAGS       = -DTRACE_DMACPY
APP_LDFLAGS      =

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif				# ASYNC

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
