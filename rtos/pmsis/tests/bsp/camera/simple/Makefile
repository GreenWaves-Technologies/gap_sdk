APP = test
APP_SRCS += test.c
APP_CFLAGS += -O3 -g

TEST_TYPE ?= quick

width = 324
height = 244

ifeq '$(TEST_TYPE)' 'fast'
APP_CFLAGS += -DTEST_TYPE_FAST
height = 20
endif

ifeq '$(TEST_TYPE)' 'quick'
APP_CFLAGS += -DTEST_TYPE_QUICK
endif

ifeq '$(TEST_TYPE)' 'long'
APP_CFLAGS += -DTEST_TYPE_LONG
endif

ifeq '$(TEST_TYPE)' 'longlong'
APP_CFLAGS += -DTEST_TYPE_LONGLONG
endif

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP8'
override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true
else
override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true
override config_args += --config-opt=**/camera/camera/image-stream=$(CURDIR)/imgTest0.raw
override config_args += --config-opt=**/camera/camera/width=$(width)
override config_args += --config-opt=**/camera/camera/height=$(height)
endif

CONFIG_HIMAX = 1

include $(RULES_DIR)/pmsis_rules.mk
