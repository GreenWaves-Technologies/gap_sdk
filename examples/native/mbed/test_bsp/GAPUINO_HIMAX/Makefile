# User Test
#------------------------------------------
TEST_C          = ../../GAPUINO_BSP/Driver/gapuino_himax.c ./ImgIO.c ./test_BSP_HIMAX.c
INC            += ../../GAPUINO_BSP/Components/himax ../../GAPUINO_BSP/Driver

# If you do not want to use RTOS, comment it.
# If you want use RTOS then uncomment it.
# MBED_FLAGS     += -DMBED_CONF_RTOS_PRESENT=1

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

# RTL Simulation
#------------------------------------------
# recordWlf=YES
# vsimDo="-do ~/wave.do"

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
