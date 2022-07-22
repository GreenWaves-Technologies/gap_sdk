APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

CONFIG_TESTBENCH=1
CONFIG_SLOW_OSC=1

ifdef SHUTDOWN_OSCILLATOR
APP_CFLAGS += -DSHUTDOWN_OSCILLATOR
endif

ifdef FIXED_WAIT
APP_CFLAGS += -DFIXED_WAIT

# When using fixed wait instead of stabilization wait, replace it by fixed wait in the efuse
override config_args += --config-opt=**/runner/efuses/content/info_1/wait_xtal=0
override config_args += --config-opt=**/runner/efuses/content/info_2/fll_wait=1
ifdef SHUTDOWN_OSCILLATOR
override config_args += --config-opt=**/runner/efuses/content/info_2/fll_wake_wait=1
endif

else

ifdef SHUTDOWN_OSCILLATOR
override config_args += --config-opt=**/runner/efuses/content/info_2/wake_wait_xtal=1
endif

endif

RAM_TYPE ?= HYPERRAM
ifeq '$(RAM_TYPE)' 'HYPERRAM'
APP_CFLAGS += -DUSE_HYPERRAM
endif

include $(RULES_DIR)/pmsis_rules.mk