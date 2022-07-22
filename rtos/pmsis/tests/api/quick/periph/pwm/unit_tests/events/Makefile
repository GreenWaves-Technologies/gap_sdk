# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_pwm_events.c
APP_INC	        +=
APP_CFLAGS      +=

override PMSIS_OS=freertos

ifdef USE_CLK_SOURCE_REF_32K
APP_CFLAGS      += -DCLK_SOURCE_REF_32K=1
endif

include $(RULES_DIR)/pmsis_rules.mk
