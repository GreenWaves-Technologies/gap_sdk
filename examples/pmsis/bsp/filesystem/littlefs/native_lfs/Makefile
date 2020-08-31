#
# LittleFS example using native LFS API
#------------------------------------

APP              = test
APP_SRCS += native_lfs.c
APP_INC +=

#
# LFS configuration
#

# Uncomment next line to active LFS traces
#APP_CFLAGS += -D LFS_YES_TRACE

# Uncomment next line to remove LFS assertions
#APP_CFLAGS += -D LFS_NO_ASSERT


#
# LFS partition configuration
#

LFS_ROOT_DIR = root

# The partition of littlefs has a size of 10MB by default.
#LFS_SIZE = 10485760
# Note: if no file has been provided, the whole  free space remaining in the flash will be used by littlefs.to the partition of littlefs.

PLPBRIDGE_FLAGS = -f


include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
