APP = jpeg_encoder
APP_SRCS += test.c
APP_INC +=
#Uncomment to compute Jpeg on cluster
#APP_CFLAGS += -DRUN_ENCODER_ON_CLUSTER=1
APP_CFLAGS += -O3 -g -w
APP_CFLAGS += -I$(TILER_EMU_INC) -I$(TILER_INC) $(CNN_LIB_INCLUDE)

## This define can be used to change JPEG Quality
## Here are listed to the possible values
## The default value (without defining any flag) is JPEG_Q_50
#APP_CFLAGS += -DJPEG_Q_0
#APP_CFLAGS += -DJPEG_Q_10
#APP_CFLAGS += -DJPEG_Q_20
#APP_CFLAGS += -DJPEG_Q_90
#APP_CFLAGS += -DJPEG_Q_90
#APP_CFLAGS += -DJPEG_Q_95


IMAGE1=imgTest0.pgm

# Include GAP_LIB/JPEG
CONFIG_GAP_LIB_JPEG = 1
# Include GAP_LIB/IMGIO
CONFIG_GAP_LIB_IMGIO = 1

io=host
# export GAP_USE_OPENOCD=1

#READFS_FILES += $(IMAGE1)

include $(RULES_DIR)/pmsis_rules.mk
