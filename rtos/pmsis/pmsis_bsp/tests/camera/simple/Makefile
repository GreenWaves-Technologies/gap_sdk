APP = test
APP_SRCS += test.c
APP_CFLAGS += -O3 -g
USE_PMSIS_BSP=1

override runner_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override runner_args += --config-opt=runner/peripherals=true


include $(RULES_DIR)/pmsis_rules.mk
