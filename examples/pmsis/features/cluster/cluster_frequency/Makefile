# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_cluster_frequency.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(ASYNC), 1)
APP_CFLAGS 	+= -DASYNC
endif

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
