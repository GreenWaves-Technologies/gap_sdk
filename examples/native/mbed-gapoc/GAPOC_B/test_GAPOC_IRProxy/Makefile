# User Test
#------------------------------------------

USER_APP_C  =   $(wildcard  ./*.c)

include ../gapoc_bsp_sources.mk

TEST_C = $(USER_APP_C) $(GAPOC_BSP_C)

# This is to store files on PC via the bridge
# The figure after -fileIO is optional (2 by default); it's a latency because program must be loaded first  
# You may comment this out if not storing file to PC
PLPBRIDGE_FLAGS += -fileIO 4

#This is to use Olimex JTAG probe
PLPBRIDGE_FLAGS += -ftdi

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk




