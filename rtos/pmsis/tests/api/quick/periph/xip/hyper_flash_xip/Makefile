# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_hyper_flash.c
APP_INC         +=
#APP_CFLAGS      += -DCLUSTER_ILL_DATA_ACCESS
#APP_CFLAGS      += -DUSE_CLUSTER

override PMSIS_OS = freertos

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC=1
endif

ifeq ($(CLUSTER), 1)
APP_CFLAGS      += -DUSE_CLUSTER=1
endif

ifeq ($(DMA), 1)
APP_CFLAGS      += -DCLUSTER_DMA=1
endif

ifeq ($(CLUSTER_ILLEGAL_ACCESS), 1)
APP_CFLAGS      += -DCLUSTER_ILL_DATA_ACCESS=1
endif

ifeq ($(XIP_RO_FC), 1)
APP_CFLAGS      += -DXIP_RO_FC=1
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
