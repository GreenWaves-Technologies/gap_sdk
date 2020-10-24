ifdef udma/cpi/version

IMPLEMENTATIONS += devices/camera/himax
devices/camera/himax_SRCS = devices/camera/himax.cpp

MAGICK=$(shell pkg-config --exists GraphicsMagick --atleast-version=1.3.23 || echo FAILED)

ifeq '$(MAGICK)' ''
ifndef NO_IMAGE_MAGICK
devices/camera/himax_CFLAGS += $(shell pkg-config GraphicsMagick --cflags)
devices/camera/himax_CFLAGS += -D__MAGICK__
devices/camera/himax_LDFLAGS += -lGraphicsMagick++ -lGraphicsMagick
endif
endif

endif