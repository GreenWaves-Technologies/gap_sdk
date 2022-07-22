APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

APP_CFLAGS += -DASYNC

ifdef RAM_TYPE
ifeq '$(RAM_TYPE)' 'HYPERRAM'
APP_CFLAGS += -DUSE_HYPERRAM
#CONFIG_HYPERRAM = 1
else
APP_CFLAGS += -DUSE_SPIRAM
# CONFIG_SPIRAM = 1
endif
endif



include $(RULES_DIR)/pmsis_rules.mk
