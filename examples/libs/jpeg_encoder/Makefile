APP = jpeg_encoder
APP_SRCS += test.c
APP_INC +=

COLOR_JPG?=0
RUN_ON_CLUSTER?=1

ifeq ($(COLOR_JPG), 1)
	APP_CFLAGS += -DCOLOR_JPG=1
	APP_CFLAGS += -DIMG_W=320
	APP_CFLAGS += -DIMG_H=240
	APP_CFLAGS += -DIMG_C=3
	APP_CFLAGS += -DIMAGE_FILE=$(CURDIR)/imgTest0.ppm
else
	APP_CFLAGS += -DIMG_W=324
	APP_CFLAGS += -DIMG_H=244
	APP_CFLAGS += -DIMG_C=1
	APP_CFLAGS += -DIMAGE_FILE=$(CURDIR)/imgTest0.pgm
endif


ifeq ($(RUN_ON_CLUSTER), 1)
	APP_CFLAGS += -DRUN_ENCODER_ON_CLUSTER=1
endif


## These defines can be used to change JPEG Quality
## Here are listed to the possible values
## The default value (without defining any flag) is JPEG_Q_50
#APP_CFLAGS += -DJPEG_Q_0
#APP_CFLAGS += -DJPEG_Q_10
#APP_CFLAGS += -DJPEG_Q_20
#APP_CFLAGS += -DJPEG_Q_90
#APP_CFLAGS += -DJPEG_Q_90
#APP_CFLAGS += -DJPEG_Q_95


# Include GAP_LIB/JPEG
CONFIG_GAP_LIB_JPEG = 1
# Include GAP_LIB/IMGIO
CONFIG_GAP_LIB_IMGIO = 1

APP_CFLAGS += -O3 -g -w
APP_CFLAGS += -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE)
io=host

clean::
	rm -f imgTest0.jpg

include $(RULES_DIR)/pmsis_rules.mk