# User Test
#------------------------------------
include ../../../bsp_src.mk

APP              = test
APP_SRCS        += hm0360_capture_simple.c $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_SRCS        += $(GAPOC_C_SRC)
APP_INC         += . $(GAP_LIB_PATH)/include $(TILER_INC)
APP_INC         += ../../../bsp $(PMSIS_BSP_PATH)/include
APP_CFLAGS      += $(GAPOC_C_BSP_FLAGS)
APP_CFLAGS      += -O3 -g

CUSTOM_BSP = 1

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk
