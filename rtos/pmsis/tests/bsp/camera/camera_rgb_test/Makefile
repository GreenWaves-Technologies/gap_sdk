# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test_camera_rgb.c
# App includes
APP_INC	         = .
# Compiler flags
APP_CFLAGS       = -O3 -g
# Linker flags
APP_LDFLAGS      =


APP_CFLAGS      += -DDEBUG_REG

ifeq ($(ASYNC), 1)
APP_CFLAGS      += -DASYNC
endif

ifeq ($(slice), 1)
APP_CFLAGS      += -DSLICE_MODE
endif


image ?= small
ifeq ($(image),small)
	APP_CFLAGS += -DSMALL_IMG
	img16 = img_16bits_small.raw 
	img24 = img_24bits_small.raw
	w = 40
	h = 40
else
	img16 = img_16bits.raw 
	img24 = img_24bits.raw
	w = 254
	h = 254
endif


format ?= rgb565
ifeq ($(format),rgb565)
APP_CFLAGS      += -DRGB565
PIC_NAME 		= $(img16)
pixel_size		= 2
else ifeq ($(format),rgb555)
APP_CFLAGS      += -DRGB555
PIC_NAME 		= $(img16)
pixel_size		= 2
else ifeq ($(format),rgb444)
APP_CFLAGS      += -DRGB444
PIC_NAME 		= $(img16)
pixel_size		= 2
else ifeq ($(format),rgb888)
APP_CFLAGS      += -DRGB888
PIC_NAME 		= $(img24)
pixel_size		= 3
endif

seq ?= rgb
ifeq ($(seq),rgb)
APP_CFLAGS += -DRGB
else ifeq ($(seq),rbg)
APP_CFLAGS += -DRBG
else ifeq ($(seq),grb)
APP_CFLAGS += -DGRB
else ifeq ($(seq),gbr)
APP_CFLAGS += -DGBR
else ifeq ($(seq),brg)
APP_CFLAGS += -DBRG
else ifeq ($(seq),bgr)
APP_CFLAGS += -DBGR
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
override config_args += --config-opt=**/camera/camera/pixel-size=$(pixel_size)
override config_args += --config-opt=**/camera/camera/vsync-polarity=1
override config_args += --config-opt=**/camera/camera/hsync-polarity=1
override config_args += --config-opt=**/camera/camera/endianness=big

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true
override config_args += --config-opt=**/camera/camera/image-stream=$(CURDIR)/$(PIC_NAME)

clean::
	@rm -rf img_OUT_*.ppm

include $(RULES_DIR)/pmsis_rules.mk
