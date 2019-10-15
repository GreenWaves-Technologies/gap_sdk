# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_cluster_callback.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(ASYNC), 1)
APP_CFLAGS 	+= -DASYNC
endif

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
