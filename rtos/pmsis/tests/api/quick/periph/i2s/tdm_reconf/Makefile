APP = test
APP_SRCS = test.c $(BUILDDIR)/use_cases.c

TEST_DURATION ?= 50
APP_CFLAGS = -O3 -g -I$(CURDIR) -DTEST_DURATION=$(TEST_DURATION)
APP_LDFLAGS = -lm -lgcc

CONFIG_I2S = 1
CONFIG_TESTBENCH=1
CONFIG_TESTBENCH_UART_ID=0

WORD_SIZE       ?= 32
ELEM_SIZE       ?= 4
BUFFER_NB_ELEM  ?= 64
FULL_DUPLEX     ?= 0
RX_SLOTS        ?= 0x1
TX_SLOTS        ?= 0
EXT_CLKS        ?= 0x0
EXT_WSS         ?= 0x0
SAI0_CLKS       ?= 0x0
SAI0_WSS        ?= 0x0
CLK_POLS        ?= 0x0
WS_POLS         ?= 0x0
USE_FAST_CLK	?= 0x0
FAST_CLK_FREQ   ?= 24576063


APP_CFLAGS += -DWS_POLS=$(WS_POLS) -DCLK_POLS=$(CLK_POLS) -DSAI0_WSS=$(SAI0_WSS) -DSAI0_CLKS=$(SAI0_CLKS) \
              -DEXT_WSS=$(EXT_WSS) -DEXT_CLKS=$(EXT_CLKS) -DWORD_SIZE=$(WORD_SIZE) -DBUFFER_NB_ELEM=$(BUFFER_NB_ELEM) \
              -DELEM_SIZE=$(ELEM_SIZE) -DFULL_DUPLEX=$(FULL_DUPLEX) -DRX_SLOTS=$(RX_SLOTS) -DTX_SLOTS=$(TX_SLOTS) \
              -DUSE_FAST_CLK=$(USE_FAST_CLK) -DFAST_CLK_FREQ=$(FAST_CLK_FREQ)


gen:
	mkdir -p $(BUILDDIR)
	./gen_use_cases.py --file=$(BUILDDIR)/use_cases.c --duration=$(TEST_DURATION)  --nb-itf=1 --sampling-freq=44100 --nb-slots=2 \
		--muted-min=10 --muted-max=100  --unmuted-min=100 --unmuted-max=200 --width-min=1 --width-max=32 --left-align=0 --left-align=1 \
		--sign-extend=0 --sign-extend=1 --msb-first=0 --msb-first=1

check:


include $(RULES_DIR)/pmsis_rules.mk
