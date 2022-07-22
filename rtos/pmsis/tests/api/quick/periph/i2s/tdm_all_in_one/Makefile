APP = test
APP_SRCS = test.c
APP_CFLAGS = -O3 -g
APP_LDFLAGS = -lm -lgcc

CONFIG_I2S = 1
CONFIG_TESTBENCH=1
CONFIG_TESTBENCH_UART_ID=0

SAMPLING_FREQ   ?= 44100
WORD_SIZE       ?= 16
ELEM_SIZE       ?= 2
NB_SAMPLE       ?= 64
BUFFER_NB_ELEM  ?= 16
NB_SLOTS        ?= 16
ITFS            ?= 0x1
FULL_DUPLEX     ?= 0
RX_SLOTS        ?= 1
TX_SLOTS        ?= 0
EXT_CLKS        ?= 0x0
EXT_WSS         ?= 0x0
SAI0_CLKS       ?= 0x0
SAI0_WSS        ?= 0x0
CLK_POLS        ?= 0x0
WS_POLS         ?= 0x0
RX_SLOTS_WIDTH_0 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
RX_SLOTS_WIDTH_1 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
TX_SLOTS_WIDTH_0 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
TX_SLOTS_WIDTH_1 ?= 0x0F0F0F0F0F0F0F0F       # 5 bits per slot, real width is specified one plus one
RX_SLOTS_FORMAT ?= 0x1111111111111111       # 4 bits per slot: bit0: MSB first, bit1: left align, bit2: sign extension
TX_SLOTS_FORMAT ?= 0x1111111111111111
USE_FAST_CLK	?= 0x0
FAST_CLK_FREQ   ?= 5000000


APP_CFLAGS += -Os -g -DRX_SLOTS_WIDTH_0=$(RX_SLOTS_WIDTH_0) -DRX_SLOTS_WIDTH_1=$(RX_SLOTS_WIDTH_1) -DTX_SLOTS_WIDTH_0=$(TX_SLOTS_WIDTH_0) -DTX_SLOTS_WIDTH_1=$(TX_SLOTS_WIDTH_1) -DRX_SLOTS_FORMAT=$(RX_SLOTS_FORMAT) -DTX_SLOTS_FORMAT=$(TX_SLOTS_FORMAT) -DWS_POLS=$(WS_POLS) -DCLK_POLS=$(CLK_POLS) -DSAI0_WSS=$(SAI0_WSS) -DSAI0_CLKS=$(SAI0_CLKS) -DEXT_WSS=$(EXT_WSS) -DEXT_CLKS=$(EXT_CLKS) -DITFS=$(ITFS) -DSAMPLING_FREQ=$(SAMPLING_FREQ) -DWORD_SIZE=$(WORD_SIZE) -DNB_SAMPLE=$(NB_SAMPLE) -DBUFFER_NB_ELEM=$(BUFFER_NB_ELEM) -DELEM_SIZE=$(ELEM_SIZE) -DFULL_DUPLEX=$(FULL_DUPLEX) -DRX_SLOTS=$(RX_SLOTS) -DTX_SLOTS=$(TX_SLOTS) -DNB_SLOTS=$(NB_SLOTS) -DUSE_FAST_CLK=$(USE_FAST_CLK) -DFAST_CLK_FREQ=$(FAST_CLK_FREQ)


INCR_END       = $(shell echo $$(((1 << $(WORD_SIZE)) - 1)))


check:
	for itf in $$(seq 0 2); do \
		if [ `echo $$(( ( $(ITFS) >> $$itf ) & 1))` == 1 ]; then \
			for slot in $$(seq 0 15); do \
				if [ `echo $$(( ( $(TX_SLOTS) >> $$slot ) & 1))` == 1 ]; then \
					echo ./check_tx_buffer.py --file $(BUILDDIR)/outfile_$${itf}_$${slot}.txt --start=`echo $$(($$slot * 2 + 1 + $${itf}*32))` --incr=96 --end=$(INCR_END) --nb-elem=$(NB_SAMPLE); \
					./check_tx_buffer.py --file $(BUILDDIR)/outfile_$${itf}_$${slot}.txt --start=`echo $$(($$slot * 2 + 1 + $${itf}*32))` --incr=96 --end=$(INCR_END) --nb-elem=$(NB_SAMPLE); if [ $$? != 0 ]; then exit 1; fi \
				fi \
			done \
		fi \
	done


include $(RULES_DIR)/pmsis_rules.mk
