APP = test
APP_SRCS = test.c
APP_CFLAGS = -O3 -g

CONFIG_I2S = 1
CONFIG_TESTBENCH=1

ifeq ($(gui), 1)
override runner_args=--gui
endif

gen:
	mkdir -p $(BUILDDIR)
	cp infile_0.txt $(BUILDDIR)
	cp infile_1.txt $(BUILDDIR)
	cp infile_2.txt $(BUILDDIR)
	cp infile_3.txt $(BUILDDIR)

include $(RULES_DIR)/pmsis_rules.mk
