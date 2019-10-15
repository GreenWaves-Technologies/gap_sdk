# User Test
#------------------------------------------
TEST_CXX      = main.cpp module_im88xx_greenwaves.cpp crc.cpp

MBED_FLAGS     += -DMBED_CONF_RTOS_PRESENT=1

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1
PLPBRIDGE_FLAGS += -ftdi -f -hyper

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
