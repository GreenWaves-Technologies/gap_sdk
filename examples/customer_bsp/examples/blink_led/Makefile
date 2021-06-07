# User Test
#------------------------------------

include $(CUSTOM_BSP_PATH)/bsp_src.mk

APP              = test
APP_SRCS         = blink_led.c
APP_SRCS        += $(GAPOC_C_SRC)
APP_INC         += $(CUSTOM_BSP_PATH)/bsp $(PMSIS_BSP_PATH)/include
APP_CFLAGS      += $(GAPOC_C_BSP_FLAGS)
APP_LDFLAGS      =

CUSTOM_BSP = 1

include $(RULES_DIR)/pmsis_rules.mk
