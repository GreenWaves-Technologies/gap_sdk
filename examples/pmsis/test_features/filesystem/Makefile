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

PLPBRIDGE_FLAGS += -f $(FLASH_FILES)

# For GVSOC
override runner_args += --config-opt=flash/fs/files=$(CURDIR)/$(FLASH_FILE_0)# \
                        --config-opt=flash/fs/files=$(CURDIR)/$(FLASH_FILE_1)

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
