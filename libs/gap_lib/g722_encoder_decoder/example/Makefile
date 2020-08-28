# User Test
#------------------------------------

APP              = test
APP_SRCS        += test.c ../src/g722_encode.c
APP_INC	        += $(TILER_INC) ../include ../../include/gaplib/
APP_CFLAGS      += -DRUNNING_ON_FC -D__FLASH_FS_SEMIHOST__

#READFS_FILES += $(CURDIR)/input_file/file.wav

include $(RULES_DIR)/pmsis_rules.mk
