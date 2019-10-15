# User Test
#------------------------------------------

USER_APP_C  =   $(wildcard  ./*.c)

include ../gapoc_bsp_sources.mk

TEST_C = $(USER_APP_C) $(GAPOC_BSP_C)


PLPBRIDGE_FLAGS += -ftdi
include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk




