# User Test
#------------------------------------------
TEST_C          = ./test_SPI_SLAVE.c

# If you do not want to use RTOS, comment it.
# If you want use RTOS then uncomment it.
# MBED_FLAGS     += -DMBED_CONF_RTOS_PRESENT=1

# For RTOS Jenkins test, it will never finished so add a jenkins test Flag to exit().
# MBED_FLAGS     +=-DJENKINS_TEST_FLAG=1

# RTL Simulation
#------------------------------------------
vsimPadMuxMode=TB_PADMUX_SPIM_SPIS_LOOPBACK

# recordWlf=YES
# vsimDo="-do ~/wave.do"

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
