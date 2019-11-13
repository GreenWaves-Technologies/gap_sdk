# User Test
#------------------------------------------
TEST_C          = ./main.c ./fft_radix8.c ./print_float.c ./utils.c

# If you do not want to use RTOS, comment it.
# If you want use RTOS then uncomment it.
# MBED_FLAGS     += -DMBED_CONF_RTOS_PRESENT=1
MBED_FLAGS     +=-DNUM_CORES=8 -O3 -g

# RTL Simulation
#------------------------------------------
# recordWlf=YES
# vsimDo="-do ~/wave.do"

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk
