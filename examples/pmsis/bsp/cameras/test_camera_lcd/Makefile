# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_camera_lcd.c $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_INC         += . $(GAP_LIB_PATH)/include $(TILER_INC)
APP_CFLAGS      += -O3 -g

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif

ifeq ($(BOARD_NAME), gapuino)
APP_CFLAGS      += -DHIMAX
endif

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=display/enabled=true

clean::
	@rm -rf img_OUT_*.ppm

include $(RULES_DIR)/pmsis_rules.mk
