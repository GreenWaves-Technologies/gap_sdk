# User Test
#------------------------------------
include $(CUSTOM_BSP_PATH)/bsp_src.mk

APP              = test
APP_SRCS        += hm0360_capture_jpeg.c $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_SRCS        += $(GAPOC_C_SRC)
APP_INC         += . $(GAP_LIB_PATH)/include $(TILER_INC)
APP_INC         += $(CUSTOM_BSP_PATH)/bsp $(PMSIS_BSP_PATH)/include
APP_CFLAGS      += $(GAPOC_C_BSP_FLAGS)
APP_CFLAGS      += -O3 -g -DIMG_QVGA

CONFIG_GAP_LIB_JPEG = 1
io=host
CUSTOM_BSP = 1

clean::
	rm -rf *.jpg

include $(RULES_DIR)/pmsis_rules.mk
