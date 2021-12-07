# User Test
#------------------------------------
include $(CUSTOM_BSP_PATH)/bsp_src.mk

APP              = test
APP_SRCS        += epeas_cis001_example.c $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_SRCS        += $(EPEAS_CIS001_BOARD_SRC)
APP_INC         += . $(GAP_LIB_PATH)/include $(TILER_INC)
APP_INC         += $(CUSTOM_BSP_PATH)/bsp $(PMSIS_BSP_PATH)/include
APP_CFLAGS      += $(EPEAS_CIS001_BOARD_BSP_FLAGS)
APP_CFLAGS      += -O3 -g
APP_CFLAGS      += -DIMG_QQVGA

PMSIS_OS=freertos

CUSTOM_BSP = 1

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk
