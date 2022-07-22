# User Test
#------------------------------------

APP_LINK_SCRIPT	 = ./GAP9_xip.ld
APP              = test
APP_SRCS        += xip_icache.c
APP_INC	        +=

ifdef CLUSTER
APP_CFLAGS      += -DTEST_CLUSTER
endif

ifdef CLUSTER_FLUSH
APP_CFLAGS      += -DCLUSTER_FLUSH
endif

ifdef FC_FLUSH
APP_CFLAGS      += -DFC_FLUSH -Os
endif

override PMSIS_OS=freertos

include $(RULES_DIR)/pmsis_rules.mk
