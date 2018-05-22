# User Test
#------------------------------------------
TEST_C          = ../../GAPUINO_BSP/Driver/gapuino_mx25r_qspi.c ./test_BSP_MX25R.c

INC            += ../../GAPUINO_BSP/Components/mx25r6435f \
		../../GAPUINO_BSP/Driver

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
