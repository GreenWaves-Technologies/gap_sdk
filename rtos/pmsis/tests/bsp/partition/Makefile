# User Test
#------------------------------------

APP              = test
APP_SRCS        +=partition.c
APP_INC         +=
APP_CFLAGS      +=

PART1_IMG = fs.img
MKFILE_DIR_PATH := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
PART1_IMG_PATH := $(MKFILE_DIR_PATH)$(PART1_IMG)

PLPBRIDGE_FLAGS = -fs "$(PART1_IMG_PATH)"

override runner_args=--config-opt=flash/raw_fs=$(PART1_IMG_PATH)

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
