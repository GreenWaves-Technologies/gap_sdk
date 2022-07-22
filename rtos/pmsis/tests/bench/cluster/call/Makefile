APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

ifdef CHECK
APP_CFLAGS += -DCHECK=1
endif

ifdef CALIB
APP_CFLAGS += -DCALIB=1
VERBOSE=0
endif

ifdef BENCH
APP_CFLAGS += -DBENCH=1
VERBOSE=0
endif

ifeq '$(VERBOSE)' '1'
APP_CFLAGS += -DVERBOSE=1
endif

include $(RULES_DIR)/pmsis_rules.mk
