# User Test
#------------------------------------


APP              = gap_dumper@${BOARD_NAME}.elf
APP_SRCS        += gap_dumper.c
APP_INC	        +=
APP_CFLAGS      +=

unexport PMSIS_OS
PMSIS_OS=freertos

include $(RULES_DIR)/pmsis_rules.mk
