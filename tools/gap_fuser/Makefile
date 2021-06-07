APP              = fuser
APP_SRCS        += gap8_fuser.c
APP_INC	        +=

dump ?= 0
test ?= 0
revc_hyper ?= 0
revb_hyper ?= 0

ifeq '$(dump)' '1'
APP_CFLAGS      += -DDUMP_REG
endif

ifeq '$(dump)' '2'
APP_CFLAGS      += -DDUMP_REG
APP_CFLAGS      += -DDUMP_ALL_REG
endif

ifeq '$(test)' '1'
APP_CFLAGS      += -DTEST_FUSER
endif

ifeq '$(revc_hyper)' '1'
APP_CFLAGS      += -DREVC_HYPER
endif

ifeq '$(revc_qspi)' '1'
APP_CFLAGS      += -DREVC_QSPI
endif


ifeq '$(revb_hyper)' '1'
APP_CFLAGS      += -DREVB_HYPER
endif




include $(RULES_DIR)/pmsis_rules.mk
