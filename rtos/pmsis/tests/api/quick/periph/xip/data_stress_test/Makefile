# User Test
#------------------------------------

APP              = test
APP_SRCS        += test.c
APP_INC         +=
#APP_CFLAGS      += -DCLUSTER_ILL_DATA_ACCESS
#APP_CFLAGS      += -DTRACE_HYPERBUS

override PMSIS_OS = freertos

#CLUSTER=1
#DMA=1

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

override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_HYPERRAM_VIP=1

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
