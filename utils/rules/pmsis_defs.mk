BOARD_NAME ?= gapuino

ifeq ($(TARGET_CHIP_FAMILY), GAP9)
PMSIS_OS ?= freertos
else
PMSIS_OS ?=pulpos
endif				# TARGET_CHIP_FAMILY

ifndef platform
ifdef PMSIS_PLATFORM
platform = $(PMSIS_PLATFORM)
else
platform = board
endif				# platform
endif				# PMSIS_PLATFORM

SHELL=bash
ECHO_GREEN = $(shell tput setaf 2)
ECHO_BOLD = $(shell tput bold)
ECHO_CLEAR = $(shell tput sgr0)

ifeq '$(PMSIS_OS)' 'pulpos'
ifeq '$(TARGET_CHIP)' 'GAP9_V2'
export USE_PULPOS=1
endif
ifeq '$(TARGET_CHIP)' 'GAP9_5'
export USE_PULPOS=1
endif
endif

# Directory containing built objects
PMSIS_OS_UPPERCASE  = $(shell echo $(PMSIS_OS) | tr a-z A-Z)
BUILDDIR            = $(CURDIR)/BUILD/$(TARGET_CHIP)/GCC_RISCV_$(PMSIS_OS_UPPERCASE)$(build_dir_ext)
TARGET_BUILD_DIR    = $(BUILDDIR)
#$(info ## BUILDDIR : $(BUILDDIR))
