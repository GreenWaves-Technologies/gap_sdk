APP = test
APP_SRCS = test.c
APP_CFLAGS = -O3 -g
APP_LDFLAGS = -lm -lgcc

CONFIG_I2S = 1
CONFIG_TESTBENCH=1
CONFIG_TESTBENCH_UART_ID=0

TEST_DURATION ?= 25

ifdef USE_GLOBAL_ENABLE
APP_CFLAGS += -DUSE_GLOBAL_ENABLE=1
endif

ifeq ($(shell test $(TEST_DURATION) -ge 100; echo $$?),0)
NB_SAMPLE = 256
else ifeq ($(shell test $(TEST_DURATION) -ge 50; echo $$?),0)
NB_SAMPLE = 128
else ifeq ($(shell test $(TEST_DURATION) -ge 25; echo $$?),0)
NB_SAMPLE = 64
else
NB_SAMPLE = 16
endif

APP_CFLAGS += -O3 -g -DNB_SAMPLE=$(NB_SAMPLE)



check:
ifdef USE_GLOBAL_ENABLE
	./check_tx_buffer_global_en.py --nb-samples=$(NB_SAMPLE) \
		--file $(BUILDDIR)/outfile_test_itf0_slot0.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot1.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot2.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot3.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot4.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot5.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot6.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot7.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot0.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot1.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot2.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot3.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot4.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot5.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot6.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot7.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot0.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot1.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot2.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot3.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot4.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot5.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot6.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot7.txt
endif
ifndef USE_GLOBAL_ENABLE
	./check_tx_buffer_slot_en.py --is-interrupted --nb-samples=$(NB_SAMPLE) \
		--file $(BUILDDIR)/outfile_test_itf0_slot0.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot2.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot4.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot6.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot0.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot2.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot4.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot6.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot0.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot2.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot4.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot6.txt
	./check_tx_buffer_slot_en.py --nb-samples=$(NB_SAMPLE) \
		--file $(BUILDDIR)/outfile_test_itf0_slot1.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot3.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot5.txt \
		--file $(BUILDDIR)/outfile_test_itf0_slot7.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot1.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot3.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot5.txt \
		--file $(BUILDDIR)/outfile_test_itf1_slot7.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot1.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot3.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot5.txt \
		--file $(BUILDDIR)/outfile_test_itf2_slot7.txt
endif

include $(RULES_DIR)/pmsis_rules.mk
