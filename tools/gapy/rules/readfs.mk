#
# ReadFS Filesystem for GAP
#
# Mathieu §Barbe - Greenwaves technologies <mathieu.barbe@grennwaves-technologies.com>
#
#
# Dependencies:
# - Gapy tool.
#
# Parameters:
# - BUILDDIR : Output directory to store generated littlefs image.
# - READFS_FILES: A list of files that will be included in the file system.
# - READFS_DIRS: A list of directories that will be included in the file system recursively.
# - READFS_DIRS_NO_REC: A list of directories that will be included in the file system not recursively.
#
# Provides:
# ReadFS partition image to $(BUILDDIR)/$(READFS_NAME), by default readfs.img.
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

MKREADFS ?= gapy gen_readfs

# Output partition image
READFS_NAME ?= readfs.img
READFS ?= $(BUILDDIR)/$(READFS_NAME)

# Partition name, must be the same that into CSV file
READFS_PARTITION_NAME ?= readfs


#
# Build dependencies
#

# Recursive wildcard function
# rwildcard(directory, pattern)
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

READFS_DEPS = \
    $(READFS_FILES) \
    $(READFS_DIRS_NO_REC) \
    $(foreach f, $(READFS_DIRS_NO_REC), $(wildcard $(f)/*)) \
    $(READFS_DIRS) \
    $(foreach f, $(READFS_DIRS), $(call rwildcard, $(f), *))

#
# ReadFS config
#

# Add ReadFS partition to flash image if files have been provided
ifneq "$(strip $(READFS_DEPS))" ""
FLASH_DEPS += $(READFS)
GEN_FLASH_IMAGE_FLAGS += -p $(READFS_PARTITION_NAME) $(READFS)
endif

#
# Targets
#

readfs: $(READFS)

$(READFS): $(READFS_DEPS) | $(BUILDDIR)
	$(MKREADFS) --comp $(READFS_FILES) --comp-dir $(READFS_DIRS_NO_REC) --comp-dir-rec $(READFS_DIRS) -o $@

clean:: readfs_clean
readfs_clean::
	rm -f $(READFS)



