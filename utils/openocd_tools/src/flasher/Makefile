# User Test
#------------------------------------

APP              = gap_flasher
APP_SRCS        += gap_flasher.c
APP_INC	        +=

ifdef MRAM
APP_CFLAGS      += -DUSE_MRAM=1
else
spiflash ?= 0
flash = $(spiflash)
APP_CFLAGS      += -DFLASH_TYPE=$(flash)
endif

include $(RULES_DIR)/pmsis_rules.mk
