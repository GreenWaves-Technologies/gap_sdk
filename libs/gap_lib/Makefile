SRC = \
  jpeg/dct.c jpeg/jpeg_constants.c jpeg/jpeg_encoder.c jpeg/cluster.c \
  img_io/ImgIO.c

CFLAGS+= -I$(TILER_EMU_INC) -I$(CURDIR)/include/gaplib

ifdef GAP_SDK_HOME
include $(CURDIR)/rules/gap_sdk.mk
else
include $(CURDIR)/rules/pulp_sdk.mk
endif
