APP = test
APP_SRCS += test.c
APP_CFLAGS += -O3 -g

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP8'
override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true
else
override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true
override config_args += --config-opt=**/camera/camera/image-stream=$(CURDIR)/imgTest0.raw
endif
 
CONFIG_HIMAX = 1



include $(RULES_DIR)/pmsis_rules.mk
