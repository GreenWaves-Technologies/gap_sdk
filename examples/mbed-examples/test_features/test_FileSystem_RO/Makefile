# User Test
#------------------------------------------
TEST_C          = ./test_FileSystem_RO.c

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

# Add hello.txt to filesystem to create binary which is written to flash.
PLPBRIDGE_FLAGS=-f hello.txt -jtag

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
