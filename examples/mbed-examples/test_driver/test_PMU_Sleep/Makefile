# User Test
#------------------------------------------
TEST_C          = ./test_PMU_Sleep.c

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

MBED_FLAGS       +=-DPRINTF_UART=1
PLPBRIDGE_FLAGS  += -f -hyper

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
