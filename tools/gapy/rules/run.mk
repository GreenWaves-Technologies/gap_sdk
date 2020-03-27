#
# Run tools for GAP
#
# Mathieu §Barbe - Greenwaves technologies <mathieu.barbe@grennwaves-technologies.com>
#
#
#
# Parameters:
# - BUILDDIR : Input directory to load generated flash image.
# - FLASH_IMAGE_NAME (optional): flash image name into $(BUILDDIR). By default $(APP).flash
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

SHELL = /bin/bash

GAPY ?= gapy

FLASH_IMAGE_NAME ?= flash.img
FLASH_IMAGE ?= $(BUILDDIR)/$(FLASH_IMAGE_NAME)

#
# Targets
#

ifeq ($(platform), gapuino)
exec:
	$(GAPY) --target $(BOARD_NAME) run --binary $(BIN) board
endif
