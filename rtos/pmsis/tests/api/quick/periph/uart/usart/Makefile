APP = test
APP_SRCS += test.c

CONFIG_TESTBENCH = 1

SOC_FREQ ?= 50000000
BAUDRATE ?= 10000000

POLARITY ?= 0
PHASE ?= 0

APP_CFLAGS += -Os -g -DSOC_FREQ=$(SOC_FREQ) -DBAUDRATE=$(BAUDRATE)
APP_LDFLAGS += -Os -g

TEST_TYPE ?= quick

ifeq '$(TEST_TYPE)' 'fast'
BUFFER_SIZE = 256
NB_ITER = 1
APP_CFLAGS += -DTEST_TYPE_FAST
endif

ifeq '$(TEST_TYPE)' 'quick'
BUFFER_SIZE = 1024
NB_ITER = 4
APP_CFLAGS += -DTEST_TYPE_QUICK
endif

ifeq '$(TEST_TYPE)' 'long'
BUFFER_SIZE = 1024
NB_ITER = 20
APP_CFLAGS += -DTEST_TYPE_LONG
endif

ifeq '$(TEST_TYPE)' 'longlong'
BUFFER_SIZE = 1024
NB_ITER = 50
APP_CFLAGS += -DTEST_TYPE_LONGLONG
endif

APP_CFLAGS += -DNB_ITER=$(NB_ITER) -DBUFFER_SIZE=$(BUFFER_SIZE) -DPOLARITY=$(POLARITY) -DPHASE=$(PHASE)

ifdef NO_BANDWIDTH_LIMITATION
APP_CFLAGS += -DNO_BANDWIDTH_LIMITATION=1
endif

ifdef USE_TRAFFIC_GEN
APP_CFLAGS += -DUSE_TRAFFIC_GEN=1
endif

override config_args += --config-opt=**/runner/jtag_mode=disable


override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_HYPERRAM_VIP=1


include $(RULES_DIR)/pmsis_rules.mk
