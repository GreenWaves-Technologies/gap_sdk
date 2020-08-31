APP              = test
APP_SRCS        += example.c
APP_INC	        +=

ifdef EFUSE_WRITE
APP_CFLAGS      += -DEFUSE_WRITE=1
endif

include $(RULES_DIR)/pmsis_rules.mk
