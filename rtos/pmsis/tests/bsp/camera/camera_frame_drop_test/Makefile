# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test_frame_drop.c
# App includes
APP_INC	         = .
# Compiler flags
APP_CFLAGS       = -O3 -g
# Linker flags
APP_LDFLAGS      =


APP_CFLAGS      += -DDEBUG_REG

img8  = img_8bits_small
img16 = img_16bits_small
img24 = img_24bits_small
w = 40
h = 40

format ?= rgb565
ifeq ($(format),rgb565)
APP_CFLAGS      += -DRGB565
PIC_NAME 		= $(img16)
pixel_size		= 2
APP_CFLAGS      += -DPIXEL_SIZE=3
else ifeq ($(format),rgb555)
APP_CFLAGS      += -DRGB555
PIC_NAME 		= $(img16)
pixel_size		= 2
APP_CFLAGS      += -DPIXEL_SIZE=3
else ifeq ($(format),rgb444)
APP_CFLAGS      += -DRGB444
PIC_NAME 		= $(img16)
pixel_size		= 2
APP_CFLAGS      += -DPIXEL_SIZE=3
else ifeq ($(format),rgb888)
APP_CFLAGS      += -DRGB888
PIC_NAME 		= $(img24)
pixel_size		= 3
APP_CFLAGS      += -DPIXEL_SIZE=3
else
APP_CFLAGS      += -DBYPASS
PIC_NAME 		= $(img8)
pixel_size		= 1
APP_CFLAGS      += -DPIXEL_SIZE=$(pixel_size)
endif

drop ?= 2
APP_CFLAGS += -DDROP=$(drop)

ifeq ($(gui), 1)
override runner_args=--gui
endif

CONFIG_HIMAX=1

PMSIS_OS = freertos

override config_args += --config-opt=**/camera/camera/width=$(w)
override config_args += --config-opt=**/camera/camera/height=$(h)
override config_args += --config-opt=**/camera/camera/pixel-size=$(pixel_size)
override config_args += --config-opt=**/camera/camera/vsync-polarity=1
override config_args += --config-opt=**/camera/camera/hsync-polarity=1
override config_args += --config-opt=**/camera/camera/endianness=big

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true
override config_args += --config-opt=**/camera/camera/image-stream=$(CURDIR)/test_img/$(PIC_NAME)%d.raw

clean::
	@rm -rf img_OUT_*.ppm

include $(RULES_DIR)/pmsis_rules.mk
