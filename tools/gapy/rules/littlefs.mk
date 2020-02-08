#
# LittleFS Filesystem for GAP
#
# Mathieu §Barbe - Greenwaves technologies <mathieu.barbe@grennwaves-technologies.com>
#
#
# Dependencies:
# - mklfs program.
#
# Parameters:
# - BUILDDIR : Output directory to store generated littlefs image.
# - LFS_ROOT_DIR : Root directory used to build filesystem.
# - BOARD_NAME : The board name to choose.
# - LFS_BLOCK_SIZE (optional) : Physical size of the erasable flash sector
# - LFS_SIZE (optional) : Size of the output filesystem, must be a multiple of block size.
#
# Provides:
# Littlefs partition image to $(BUILDDIR)/$(LFS_NAME), by default lfs.img.
#


ifndef LFS_ROOT_DIR

lfs:
	@echo LFS_ROOT_DIR variable must be specified to build LFS image.

else

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

ifndef BOARD_NAME
$(error Board name undefined (BOARD_NAME undefined).)
else
ifeq ("$(BOARD_NAME)", "")
$(error Board name undefined (BOARD_NAME is empty).)
endif
endif

SHELL = /bin/bash

# Output partition image
LFS_NAME ?= lfs.img
LFS ?= $(BUILDDIR)/$(LFS_NAME)

# Partition name, must be the same that into CSV file
LFS_PARTITION_NAME ?= lfs

#
# Build dependencies
#

# Recursive wildcard
# rwildcard(directory, pattern)
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

LFS_DEPS = $(call rwildcard, $(LFS_ROOT_DIR), *)

#
# LittleFS config
#

MKLFS ?= mklfs

#
# Sector size and FS size
#

ifeq ($(BOARD_NAME), gapoca)

# Flash type : SPI flash - Flash size : 16 MiB
# Block size 4 KiB
LFS_BLOCK_SIZE ?= 4096

else

# Flash type : Hyperflash - Flash size : 128 Mio
# Block size 256 KiB
LFS_BLOCK_SIZE ?= 262144

endif

# Filesytem size : 10 MiB by default
LFS_SIZE ?= 10485760

# Minimum data length and alignement constraint for flash operations
LFS_READ_SIZE = 4
LFS_PROG_SIZE = 4

# Add littlefs partition to flash image if files have been provided
ifneq "$(strip $(LFS_DEPS))" ""
FLASH_DEPS += $(LFS)
GEN_FLASH_IMAGE_FLAGS += -p $(LFS_PARTITION_NAME) $(LFS)
endif

#
# Targets
#

lfs: $(LFS)
$(LFS): $(LFS_ROOT_DIR) $(LFS_DEPS) | $(BUILDDIR)
	$(MKLFS) -b $(LFS_BLOCK_SIZE) -r $(LFS_READ_SIZE) -p $(LFS_PROG_SIZE) -s $(LFS_SIZE) -c $< -i $@

clean:: lfs_clean
lfs_clean:
	rm -f $(LFS)

endif
