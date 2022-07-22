APP = test
APP_SRCS = test.c kiss_fft.c
APP_CFLAGS = -O3 -g
APP_LDFLAGS = -lm -lgcc

CONFIG_I2S = 1
CONFIG_TESTBENCH_UART_ID=0
#RUNNER_CONFIG = $(CURDIR)/config.ini

ifdef GAP_SDK_HOME
CONFIG_BUILD_DIR = $(BUILDDIR)
endif

#RUNNER_CONFIG = $(CURDIR)/config.ini

STIM_WAV_0_0 ?= $(CONFIG_BUILD_DIR)/stim_0_0.wav
STIM_WAV_0_1 ?= $(CONFIG_BUILD_DIR)/stim_0_1.wav
STIM_WAV_0_2 ?= $(CONFIG_BUILD_DIR)/stim_0_2.wav
STIM_WAV_0_3 ?= $(CONFIG_BUILD_DIR)/stim_0_3.wav

TX_ENABLED ?= 1
RX_ENABLED ?= 1
LOOPBACK_ENABLED ?= 0

SAMPLING_FREQ_0 ?= 44100
SAMPLING_FREQ_1 ?= 44100

SIGNAL_FREQ_0_0 ?= 800
SIGNAL_FREQ_0_1 ?= 1400
SIGNAL_FREQ_0_2 ?= 2000
SIGNAL_FREQ_0_3 ?= 2600

PDM ?= 1
NB_ITF ?= 1
NB_CHANNELS ?= 1
WORD_SIZE ?= 24
TDM ?= 0
NB_ELEM ?= 256

ifeq '$(TX_ENABLED)' '1'
incr = $(shell echo $$((((1<<$(WORD_SIZE)) / $(NB_ELEM)) * 7 + 13)))
start = 0
APP_CFLAGS += -DTX_ENABLED -DTX_BUFFER_START0_0=$(start) -DTX_BUFFER_NEXT0_0=$(incr)
endif

ifeq '$(RX_ENABLED)' '1'
APP_CFLAGS += -DRX_ENABLED
endif

ifeq '$(LOOPBACK_ENABLED)' '1'
APP_CFLAGS += -DLOOPBACK_ENABLED
endif


override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true


override config_args += --config-opt=microphone0/width=$(WORD_SIZE)
override config_args += --config-opt=board/components/mic0/mic/enabled=true

override config_args += --config-opt=microphone0/stim_0=$(STIM_WAV_0_0)
override config_args += --config-opt=microphone0/stim_1=$(STIM_WAV_0_1)
override config_args += --config-opt=microphone0/stim_2=$(STIM_WAV_0_2)
override config_args += --config-opt=microphone0/stim_3=$(STIM_WAV_0_3)

override config_args += --config-opt=board/components/sink0/sink/outfile=outfile.hex
override config_args += --config-opt=board/components/sink0/sink/width=$(WORD_SIZE)

use_fft = 1

ifeq '$(use_fft)' '1'
override config_args += --config-opt=board/components/mic0/mic/stim=$(STIM_WAV_0_0)
APP_CFLAGS += -DUSE_FFT=1
else
incr0 = $(shell echo $$((((1<<$(WORD_SIZE)) / $(NB_ELEM)) * 3 + 1)))
incr1 = $(shell echo $$((((1<<$(WORD_SIZE)) / $(NB_ELEM)) * 4 + 16)))
incr2 = $(shell echo $$((((1<<$(WORD_SIZE)) / $(NB_ELEM)) * 5 + 392)))
incr3 = $(shell echo $$((((1<<$(WORD_SIZE)) / $(NB_ELEM)) * 6 + 8765)))

start0 = 0
start1 = 11
start2 = 271
start3 = 3931

incr0 = 0
incr1 = 0
incr2 = 0
incr3 = 0

start0 = 0x5555
start1 = 0x5555
start2 = 0x5555
start3 = 0x5555

override config_args += --config-opt=board/components/mic0/mic/stim_mode=incr
override config_args += --config-opt=board/components/mic0/mic/stim_incr_start=$(start0)
override config_args += --config-opt=board/components/mic0/mic/stim_incr_value=$(incr0)

override config_args += --config-opt=board/components/mic1/mic/stim_mode=incr
override config_args += --config-opt=board/components/mic1/mic/stim_incr_start=$(start1)
override config_args += --config-opt=board/components/mic1/mic/stim_incr_value=$(incr1)

override config_args += --config-opt=board/components/mic2/mic/stim_mode=incr
override config_args += --config-opt=board/components/mic2/mic/stim_incr_start=$(start2)
override config_args += --config-opt=board/components/mic2/mic/stim_incr_value=$(incr2)

override config_args += --config-opt=board/components/mic3/mic/stim_mode=incr
override config_args += --config-opt=board/components/mic3/mic/stim_incr_start=$(start3)
override config_args += --config-opt=board/components/mic3/mic/stim_incr_value=$(incr3)

APP_CFLAGS += \
	-DINCR_VALUE0_0=$(incr0) -DINCR_START0_0=$(start0) \
	-DINCR_VALUE0_1=$(incr1) -DINCR_START0_1=$(start1) \
	-DINCR_VALUE0_2=$(incr2) -DINCR_START0_2=$(start2) \
	-DINCR_VALUE0_3=$(incr3) -DINCR_START0_3=$(start3)

endif


APP_CFLAGS += -O3 -g -DSAMPLING_FREQ_0=$(SAMPLING_FREQ_0) -DSAMPLING_FREQ_1=$(SAMPLING_FREQ_1) \
	-DSIGNAL_FREQ_0_0=$(SIGNAL_FREQ_0_0) -DSIGNAL_FREQ_0_1=$(SIGNAL_FREQ_0_1) -DSIGNAL_FREQ_0_2=$(SIGNAL_FREQ_0_2) \
	-DSIGNAL_FREQ_0_3=$(SIGNAL_FREQ_0_3) -DPDM=$(PDM) -DNB_ITF=$(NB_ITF) -DNB_CHANNELS=$(NB_CHANNELS) \
	-DWORD_SIZE=$(WORD_SIZE) -DTDM=$(TDM) -DNB_ELEM=$(NB_ELEM)

gen:
	sox -n -r $(SAMPLING_FREQ_0) --bits $(WORD_SIZE) $(STIM_WAV_0_0) synth 3 sine $(SIGNAL_FREQ_0_0) vol 0.995
	sox -n -r $(SAMPLING_FREQ_0) --bits $(WORD_SIZE) $(STIM_WAV_0_1) synth 3 sine $(SIGNAL_FREQ_0_1) vol 0.995
	sox -n -r $(SAMPLING_FREQ_0) --bits $(WORD_SIZE) $(STIM_WAV_0_2) synth 3 sine $(SIGNAL_FREQ_0_2) vol 0.995
	sox -n -r $(SAMPLING_FREQ_0) --bits $(WORD_SIZE) $(STIM_WAV_0_3) synth 3 sine $(SIGNAL_FREQ_0_3) vol 0.995

check:
	./script.py --file $(BUILDDIR)/outfile.hex --sample-rate=$(SAMPLING_FREQ_0) --frequency=$(SIGNAL_FREQ_0_0)


include $(RULES_DIR)/pmsis_rules.mk
