#
# flash tools for GAP
#
# Mathieu §Barbe - Greenwaves technologies <mathieu.barbe@grennwaves-technologies.com>
#
#
# Dependencies:
# - gen_flash_image and flasher or gapy tool.
#
# Parameters:
# - BUILDDIR : Output directory to store generated flash image.
# - FLASH_IMAGE_NAME (optional): flash image name into $(BUILDDIR). By default $(APP).flash
#
# Provides:
# FLASH_IMAGE : build flash image with
#

#
# Check parameters
#

ifndef BUILDDIR
$(error Build output directory undefined (BUILDDIR undefined).)
else
ifeq ("$(BUILDDIR)", "")
$(error Build output directory undefined (BUILDDIR is empty).)
endif
endif

ifndef BIN
$(error Bin input file undefined (BIN undefined).)
else
ifeq ("$(BIN)", "")
$(error Bin input file undefined (BIN is empty).)
endif
endif


SHELL = /bin/bash

GAPY ?= gapy

FLASH_IMAGE_NAME ?= flash.img
FLASH_IMAGE ?= $(BUILDDIR)/$(FLASH_IMAGE_NAME)

#
# Runner hack to flash image
#
RAW_IMAGE_PLPBRIDGE_FLAGS = -i $(FLASH_IMAGE)

#
# Targets
#

all:: flash_image
flash_image: $(FLASH_IMAGE)

$(FLASH_IMAGE): $(BIN) $(FLASH_DEPS)
	$(GAPY) gen_flash_image --boot-loader $< $(GEN_FLASH_IMAGE_FLAGS) -o $@ --flash-size 64M --block-size 256K --flash-type hyper
