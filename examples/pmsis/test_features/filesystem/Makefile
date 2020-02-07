# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_filesystem.c
APP_INC         +=
APP_CFLAGS      +=

ifeq ($(CLUSTER), 1)
APP_CFLAGS      += -DUSE_CLUSTER
endif

FLASH_FILE_0 = hello.txt
FLASH_FILE_1 = Makefile

FLASH_FILES = $(FLASH_FILE_0) #$(FLASH_FILE_1)

# list of files to include in the ReadFS.
READFS_FILES := $(FLASH_FILES)

# list of directories to include in the ReadFS.
READFS_DIRS :=

# To flash image before run, add -f flag to the bridge.
PLPBRIDGE_FLAGS += -f


include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
