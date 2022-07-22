APP        += test_ffc_i2s_send
APP_SRCS   += test_ffc_i2s_send.c
#APP_CFLAGS += -DTRACE_I2S=1
APP_CFLAGS +=

CONFIG_I2S = 1
CONFIG_TESTBENCH=1
CONFIG_TESTBENCH_UART_ID=0

ifdef FFC_ID
APP_CFLAGS += -DFFC_ID=$(FFC_ID)
endif

check:
	./verify.sh $(BUILDDIR)

include $(RULES_DIR)/pmsis_rules.mk
