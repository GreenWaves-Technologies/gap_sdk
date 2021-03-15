# User Test
#------------------------------------

APP              = gap_flasher
APP_SRCS        += gap_flasher.c
APP_INC	        +=

spiflash ?= 0
flash = $(spiflash)

APP_CFLAGS      += -DFLASH_TYPE=$(flash)

include $(RULES_DIR)/pmsis_rules.mk
