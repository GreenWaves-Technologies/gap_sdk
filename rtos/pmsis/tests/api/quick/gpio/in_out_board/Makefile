# User Test
#------------------------------------

APP              = test
APP_SRCS        += gpio.c
APP_INC	        +=
APP_CFLAGS      +=

gpio_in=1
gpio_out=1

ifeq ($(gpio_in), 1)
APP_CFLAGS += -DGPIO_INPUT
endif

ifeq ($(gpio_out), 1)
APP_CFLAGS += -DGPIO_OUTPUT
endif

include $(RULES_DIR)/pmsis_rules.mk
