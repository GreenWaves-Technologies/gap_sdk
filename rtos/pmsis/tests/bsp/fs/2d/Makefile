FS_TYPE ?= read_fs

USE_PMSIS_BSP=1

FILES = $(CURDIR)/files/flash_file_0.bin $(CURDIR)/files/flash_file_1.bin

ifdef FLASH_TYPE
ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'
APP_CFLAGS += -DUSE_HYPERFLASH
else
ifeq '$(FLASH_TYPE)' 'MRAM'
APP_CFLAGS += -DUSE_MRAM
READFS_FLASH = target/chip/soc/mram
else
APP_CFLAGS += -DUSE_SPIFLASH
CONFIG_ATXP032 = 1
READFS_FLASH = target/board/devices/spiflash
endif
endif
APP_CFLAGS += -DFLASH_TYPE=$(FLASH_TYPE)
endif


ifeq '$(FS_TYPE)' 'lfs'
LFS_ROOT_DIR = files
PLPBRIDGE_FLAGS += -f -jtag
APP_CFLAGS += -DFS_LFS
else
ifeq '$(FS_TYPE)' 'host'
HOSTFS_FILES = $(FILES)
APP_CFLAGS += -DFS_HOST
export GAP_USE_OPENOCD=1
else
READFS_FILES = $(FILES)
PLPBRIDGE_FLAGS += -f -jtag
APP_CFLAGS += -DFS_READ_FS
endif
endif

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

ifdef USE_CLUSTER
APP_CFLAGS += -DUSE_CLUSTER
endif


include $(RULES_DIR)/pmsis_rules.mk
