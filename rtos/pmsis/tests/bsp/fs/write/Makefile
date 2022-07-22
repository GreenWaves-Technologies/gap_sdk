-include $(RULES_DIR)/properties.mk


FS_TYPE ?= read_fs

USE_PMSIS_BSP=1

FILE0_NAME = flash_file_0.bin
FILE0 = files/$(FILE0_NAME)
FILE1_NAME = flash_file_1.bin
FILE1 = files/$(FILE1_NAME)
TX_FILE0 = tx_flash_file_0.bin
TX_FILE1 = tx_flash_file_1.bin
FILES = $(FILE0) $(FILE1)

ifeq '$(FS_TYPE)' 'lfs'
LFS_ROOT_DIR = files
PLPBRIDGE_FLAGS += -f -jtag
FILE0_PATH = $(FILE0_NAME)
FILE1_PATH = $(FILE1_NAME)
APP_CFLAGS += -DFS_LFS
else
ifeq '$(FS_TYPE)' 'host'
FILE0_PATH = $(CURDIR)/$(FILE0)
FILE1_PATH = $(CURDIR)/$(FILE1)
APP_CFLAGS += -DFS_HOST
export GAP_USE_OPENOCD=1
else
PLPBRIDGE_FLAGS += -f $(FILES) -jtag
override runner_args += $(foreach file,$(FILES), --config-opt=flash/fs/files=$(CURDIR)/$(file))
FILE0_PATH = $(FILE0)
FILE1_PATH = $(FILE1)
APP_CFLAGS += -DFS_READ_FS
endif
endif

TX_FILE0_PATH = $(TX_FILE0)
TX_FILE1_PATH = $(TX_FILE1)

APP_CFLAGS += -DFILE0="$(FILE0_PATH)"
APP_CFLAGS += -DFILE1="$(FILE1_PATH)"
APP_CFLAGS += -DTX_FILE0="$(TX_FILE0_PATH)"
APP_CFLAGS += -DTX_FILE1="$(TX_FILE1_PATH)"

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

ifdef USE_CLUSTER
APP_CFLAGS += -DUSE_CLUSTER
endif


include $(RULES_DIR)/pmsis_rules.mk
