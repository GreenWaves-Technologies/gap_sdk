FS_TYPE ?= read_fs

USE_PMSIS_BSP=1

FILE0_NAME = flash_file_0.bin
FILE0 = files/$(FILE0_NAME)
FILE1_NAME = flash_file_1.bin
FILE1 = files/$(FILE1_NAME)
FILES = $(FILE0) $(FILE1)
FILE0_PATH = $(FILE0_NAME)
FILE1_PATH = $(FILE1_NAME)

ifdef FLASH_TYPE
ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'
APP_CFLAGS += -DUSE_HYPERFLASH
else
ifeq '$(FLASH_TYPE)' 'MRAM'
APP_CFLAGS += -DUSE_MRAM
READFS_FLASH = target/chip/soc/mram
else
APP_CFLAGS += -DUSE_SPIFLASH
READFS_FLASH = target/board/devices/spiflash
endif
endif
APP_CFLAGS += -DFLASH_TYPE=$(FLASH_TYPE)
endif


ifeq '$(FS_TYPE)' 'lfs'
LFS_ROOT_DIR = files
PLPBRIDGE_FLAGS += -f -jtag
APP_CFLAGS += -DFS_LFS
CONFIG_LFS = 1
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

APP_CFLAGS += -DFILE0="$(FILE0_PATH)"
APP_CFLAGS += -DFILE1="$(FILE1_PATH)"

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

ifdef USE_CLUSTER
APP_CFLAGS += -DUSE_CLUSTER
endif


include $(RULES_DIR)/pmsis_rules.mk
