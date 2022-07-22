SHELL = /bin/bash

APP = test
APP_SRCS = test.c
APP_CFLAGS = -O3 -g
APP_LDFLAGS = -lm -lgcc

CONFIG_I2S = 1
CONFIG_TESTBENCH=1
ifeq ($(GAP9_WLCSP), 1)
CONFIG_TESTBENCH_UART_ID=0
endif

WS_DELAYS        ?= 0x111
SAMPLING_FREQ_0  ?= 48000
SAMPLING_FREQ_1  ?= 48000
SAMPLING_FREQ_2  ?= 48000
WORD_SIZE        ?= 16
ELEM_SIZE        ?= 2
BUFFER_NB_ELEM   ?= 16
NB_SLOTS         ?= 16
ITFS             ?= 0x1
FULL_DUPLEX      ?= 0
RX_SLOTS         ?= 1
TX_SLOTS         ?= 0
EXT_CLKS         ?= 0x0
EXT_WSS          ?= 0x0
SAI0_CLKS        ?= 0x0
SAI0_WSS         ?= 0x0
CLK_POLS         ?= 0x0
WS_POLS          ?= 0x0
RX_SLOTS_WIDTH_0 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
RX_SLOTS_WIDTH_1 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
RX_SLOTS_BYPASS  ?= 0
TX_SLOTS_WIDTH_0 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
TX_SLOTS_WIDTH_1 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
RX_SLOTS_FORMAT  ?= 0x1111111111111111       # 4 bits per slot: bit0: MSB first, bit1: left align, bit2: sign extension
TX_SLOTS_FORMAT  ?= 0x1111111111111111
TX_SLOTS_BYPASS  ?= 0
USE_FAST_CLK	 ?= 0x0
FAST_CLK_FREQ    ?= 24576063 #5000000        # Setting 24MHz as default(driver should be aware of frequency input)
MUTE             ?= 0
INJECT_STALL     ?= 0
NB_ITER          ?= 1
INCR             ?= 0
TX_FRAMES        ?= 0x0000000000000000
RX_FRAMES        ?= 0x0000000000000000
TX_SLABS         ?= 0x0000000000000000
RX_SLABS         ?= 0x0000000000000000

TEST_DURATION ?= 25

ifeq ($(shell test $(TEST_DURATION) -ge 100; echo $$?),0)
NB_SAMPLE = 256
else ifeq ($(shell test $(TEST_DURATION) -ge 50; echo $$?),0)
NB_SAMPLE = 128
else ifeq ($(shell test $(TEST_DURATION) -ge 25; echo $$?),0)
NB_SAMPLE = 64
else
NB_SAMPLE = 16
endif

APP_CFLAGS += -O3 -g -DNB_ITER=$(NB_ITER) -DINJECT_STALL=$(INJECT_STALL) -DMUTE=$(MUTE) -DRX_SLOTS_WIDTH_0=$(RX_SLOTS_WIDTH_0)
APP_CFLAGS += -DRX_SLOTS_WIDTH_1=$(RX_SLOTS_WIDTH_1) -DTX_SLOTS_WIDTH_0=$(TX_SLOTS_WIDTH_0) -DTX_SLOTS_WIDTH_1=$(TX_SLOTS_WIDTH_1)
APP_CFLAGS += -DRX_SLOTS_FORMAT=$(RX_SLOTS_FORMAT) -DTX_SLOTS_FORMAT=$(TX_SLOTS_FORMAT) -DWS_POLS=$(WS_POLS) -DCLK_POLS=$(CLK_POLS)
APP_CFLAGS += -DSAI0_WSS=$(SAI0_WSS) -DSAI0_CLKS=$(SAI0_CLKS) -DEXT_WSS=$(EXT_WSS) -DEXT_CLKS=$(EXT_CLKS) -DITFS=$(ITFS)
APP_CFLAGS += -DSAMPLING_FREQ_0=$(SAMPLING_FREQ_0) -DSAMPLING_FREQ_1=$(SAMPLING_FREQ_1) -DSAMPLING_FREQ_2=$(SAMPLING_FREQ_2) -DWORD_SIZE=$(WORD_SIZE) -DNB_SAMPLE=$(NB_SAMPLE) -DBUFFER_NB_ELEM=$(BUFFER_NB_ELEM)
APP_CFLAGS += -DELEM_SIZE=$(ELEM_SIZE) -DFULL_DUPLEX=$(FULL_DUPLEX) -DRX_SLOTS=$(RX_SLOTS) -DTX_SLOTS=$(TX_SLOTS) -DNB_SLOTS=$(NB_SLOTS)
APP_CFLAGS += -DUSE_FAST_CLK=$(USE_FAST_CLK) -DFAST_CLK_FREQ=$(FAST_CLK_FREQ) -DTX_SLOTS_BYPASS=$(TX_SLOTS_BYPASS) -DRX_SLOTS_BYPASS=$(RX_SLOTS_BYPASS)
APP_CFLAGS += -DWS_DELAYS=$(WS_DELAYS) -DINCR=$(INCR) -DTX_FRAMES=$(TX_FRAMES) -DRX_FRAMES=$(RX_FRAMES) -DTX_SLABS=$(TX_SLABS) -DRX_SLABS=$(RX_SLABS)

ifdef I2S_FAST_CLOCK
override config_args += --config-opt=**/rtl/args=+ENABLE_EXT_FAST_CLOCK=1
override config_args += --config-opt=**/rtl/args=+DISABLE_FAST_CLOCK=1
ifeq '$(TARGET_CHIP)' 'GAP9_5'
override config_args += --config-opt=**/runner/efuses/content/info_1/osc_ctrl=2
override config_args += --config-opt=**/runner/efuses/content/info_2/wake_osc_ctrl=2
override config_args += --config-opt=**/runner/efuses/content/padfun4=0x00000100
else
override config_args += --config-opt=**/runner/efuses/content/info_1/osc_ctrl=4
override config_args += --config-opt=**/runner/efuses/content/info_2/wake_osc_ctrl=4
override config_args += --config-opt=**/runner/efuses/content/padfun4=0x00000001
endif
CONFIG_I2S_FAST_CLOCK = 1
endif


INCR_END       = $(shell echo $$(((1 << $(WORD_SIZE)) - 1)))

ifneq '$(MUTE)' '0'
MUTE_OPT = --mute
endif

ifeq '$(INJECT_STALL)'  '1'
check:
else
check:
	for itf in $$(seq 0 2); do \
		if [ `echo $$(( ( $(ITFS) >> $$itf ) & 1))` == 1 ]; then \
			for slot in $$(seq 0 15); do \
				if [ `echo $$(( ( $(TX_SLOTS) >> $$slot ) & 1))` == 1 ]; then \
					echo ./check_tx_buffer.py --file $(BUILDDIR)/outfile_test0_itf$${itf}_slot$${slot}.txt --word-size=$(WORD_SIZE) \
						--format=`echo $$(( (( $(TX_SLOTS_FORMAT) >> ($$slot * 4)) & 0xf)))` --slot-size-0=`echo $$(( (( $(TX_SLOTS_WIDTH_0) >> ($$slot * 8) ) & 0x1f) + 1))` \
						--slot-size-1=`echo $$(( (( $(TX_SLOTS_WIDTH_1) >> ($$slot * 8) ) & 0x1f) + 1))` \
						--start=`echo $$(($$slot * 2 + !(( $(TX_SLOTS_BYPASS) >> ($$slot ) ) & 1) + $${itf}*32))` --incr=`echo $$(($(NB_SLOTS) * 3 * ($(FULL_DUPLEX) + 1)))` \
						--end=$(INCR_END) --nb-elem=$(NB_SAMPLE) $(MUTE_OPT) --slot-id=$$slot; \
					./check_tx_buffer.py --file $(BUILDDIR)/outfile_test0_itf$${itf}_slot$${slot}.txt --word-size=$(WORD_SIZE) \
						--format=`echo $$(( (( $(TX_SLOTS_FORMAT) >> ( $$slot * 4) ) & 0xf)))` --slot-size-0=`echo $$(( (( $(TX_SLOTS_WIDTH_0) >> ($$slot * 8) ) & 0x1f) + 1))` \
						--slot-size-1=`echo $$(( (( $(TX_SLOTS_WIDTH_1) >> ($$slot * 8) ) & 0x1f) + 1))` \
						--start=`echo $$(($$slot * 2 + !(( $(TX_SLOTS_BYPASS) >> ($$slot ) ) & 1) + $${itf}*32))` --incr=`echo $$(($(NB_SLOTS) * 3 * ($(FULL_DUPLEX) + 1)))` \
						--end=$(INCR_END) --nb-elem=$(NB_SAMPLE) $(MUTE_OPT) --slot-id=$$slot ; if [ $$? != 0 ]; then exit 1; fi \
				fi \
			done \
		fi \
	done
endif

include $(RULES_DIR)/pmsis_rules.mk
