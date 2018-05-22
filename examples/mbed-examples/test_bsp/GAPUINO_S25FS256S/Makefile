# User Test
#------------------------------------------
TEST_C          = ../../GAPUINO_BSP/Driver/gapuino_s25fs_qspi.c ./test_BSP_S25FS.c

INC            += ../..//GAPUINO_BSP/Components/s25fs256s \
		../../GAPUINO_BSP/Driver

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
