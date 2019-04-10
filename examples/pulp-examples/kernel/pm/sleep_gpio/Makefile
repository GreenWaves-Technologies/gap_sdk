PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_APP_HOST_SRCS = test.c
PULP_CFLAGS = -O3 -g

PULP_USER_CONFIG = $(CURDIR)/config.json
PULP_CFLAGS += -DSPIM_ITF=0
PULP_CFLAGS += -DSPIM_CS=1

override CONFIG_OPT += runner/peripherals=true


include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
