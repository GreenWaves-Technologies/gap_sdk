APP_CFLAGS += -DUSE_MRAM

ifdef CALIB
APP_CFLAGS += -DCALIB=1
endif

APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g
APP_LDFLAGS += -O3 -g

include $(RULES_DIR)/pmsis_rules.mk
