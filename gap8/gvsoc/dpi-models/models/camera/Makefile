DPI_MODELS += camera

camera_SRCS = camera/camera.cpp

MAGICK_PKG_CFG_CMD := pkg-config
ifdef MAGICK_ROOT
# MAGICK_ROOT can be set to specify the root directory (containing e.g. /lib/,
# /include/ etc) where GraphicsMagick was installed from source
MAGICK_PKG_CFG_PATH := $(MAGICK_ROOT)/lib/pkgconfig:$(PKG_CONFIG_PATH)
camera_CFLAGS += -Wl,-rpath=$(MAGICK_ROOT)/lib -I$(MAGICK_ROOT)/include/GraphicsMagick
MAGICK_PKG_CFG_CMD := PKG_CONFIG_PATH=$(MAGICK_PKG_CFG_PATH) pkg-config
endif

MAGICK=$(shell $(MAGICK_PKG_CFG_CMD)  --exists GraphicsMagick --atleast-version=1.3.23 || echo FAILED)


ifeq '$(MAGICK)' ''
camera_CFLAGS += $(shell $(MAGICK_PKG_CFG_CMD) GraphicsMagick++ --cflags)
camera_CFLAGS += -D__MAGICK__
camera_LDFLAGS := $(shell $(MAGICK_PKG_CFG_CMD) GraphicsMagick++ --libs)
#LDFLAGS += $(shell Magick++-config --ldflags)
endif
