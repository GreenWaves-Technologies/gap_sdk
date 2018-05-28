# User Test
#------------------------------------------
TEST_C          = ./test_I2S.c

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

PLPBRIDGE_FLAGS += -s i2s@../../../i2s.py

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
