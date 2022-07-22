# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test_camera_bypass.c
# App includes
APP_INC	         = .
# Compiler flags
APP_CFLAGS       = -O3 -g
# Linker flags
APP_LDFLAGS      =


APP_CFLAGS      += -DDEBUG_REG
#APP_CFLAGS      += -DSAVE_PIC

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif

ifeq ($(test), 1)
APP_CFLAGS      += -DTEST_PIC
endif

ifeq ($(slice), 1)
APP_CFLAGS      += -DSLICE_MODE
endif

vpol ?= 1
APP_CFLAGS      += -DVPOL=$(vpol)
hpol ?= 1
APP_CFLAGS      += -DHPOL=$(hpol)

image ?= small
ifeq ($(image),small)
    APP_CFLAGS += -DSMALL_IMG
    img8  = img_8bits_small
    img16 = img_16bits_small
    w = 40
    h = 40
else
    img8  = img_8bits
    img16 = img_16bits
    w = 254
    h = 254
endif

pixel ?= 1
ifeq ($(pixel),2)
APP_CFLAGS      += -DPIXEL_SIZE=2
PIC_NAME 		= $(img16).raw
else
APP_CFLAGS      += -DPIXEL_SIZE=1
PIC_NAME 		= $(img8).raw
endif

ifeq ($(gui), 1)
override runner_args=--gui
endif

# Include GAP_LIB/IMGIO
#CONFIG_GAP_LIB_IMGIO = 1
CONFIG_HIMAX=1

PMSIS_OS = freertos

override config_args += --config-opt=**/camera/camera/width=$(w)
override config_args += --config-opt=**/camera/camera/height=$(h)
override config_args += --config-opt=**/camera/camera/pixel-size=$(pixel)
override config_args += --config-opt=**/camera/camera/vsync-polarity=$(vpol)
override config_args += --config-opt=**/camera/camera/hsync-polarity=$(hpol)

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true
override config_args += --config-opt=**/camera/camera/image-stream=$(CURDIR)/test_img/$(PIC_NAME)

clean::
	@rm -rf img_OUT_*.ppm

include $(RULES_DIR)/pmsis_rules.mk
