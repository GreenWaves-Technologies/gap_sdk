APP = test
APP_SRCS = test.c kiss_fft.c
APP_CFLAGS = -O3 -g
APP_LDFLAGS = -lm -lgcc

ifdef GAP_SDK_HOME
CONFIG_BUILD_DIR = $(BUILDDIR)
endif

RUNNER_CONFIG = $(CURDIR)/config.ini

STIM_WAV_0_0 ?= $(CONFIG_BUILD_DIR)/stim_0_0.wav
STIM_WAV_0_1 ?= $(CONFIG_BUILD_DIR)/stim_0_1.wav
STIM_WAV_1_0 ?= $(CONFIG_BUILD_DIR)/stim_1_0.wav
STIM_WAV_1_1 ?= $(CONFIG_BUILD_DIR)/stim_1_1.wav

SAMPLING_FREQ_0 ?= 44100
SAMPLING_FREQ_1 ?= 44100

SIGNAL_FREQ_0_0 ?= 800
SIGNAL_FREQ_0_1 ?= 1400
SIGNAL_FREQ_1_0 ?= 2000
SIGNAL_FREQ_1_1 ?= 2600

PDM ?= 1
NB_ITF ?= 1
NB_CHANNELS ?= 1
WORD_SIZE ?= 16
NB_ELEM ?= 256
NB_CAPTURE ?= 16
NB_START_STOP ?= 4

override config_args += --config-opt=microphone0/width=$(WORD_SIZE)
override config_args += --config-opt=microphone1/width=$(WORD_SIZE)

override config_args += --config-opt=microphone0/stim_left=$(STIM_WAV_0_0)
override config_args += --config-opt=microphone0/stim_right=$(STIM_WAV_0_1)
override config_args += --config-opt=microphone1/stim_left=$(STIM_WAV_1_0)
override config_args += --config-opt=microphone1/stim_right=$(STIM_WAV_1_1)

ifeq '$(PDM)' '1'
override config_args += --config-opt=microphone0/pdm=true
override config_args += --config-opt=microphone1/pdm=true

ifeq '$(NB_CHANNELS)' '2'
override config_args += --config-opt=microphone0/ddr=true
override config_args += --config-opt=microphone1/ddr=true
else
override config_args += --config-opt=microphone0/ddr=false
override config_args += --config-opt=microphone1/ddr=false
endif


else
override config_args += --config-opt=microphone0/pdm=false
override config_args += --config-opt=microphone1/pdm=false
ifeq '$(NB_CHANNELS)' '2'
override config_args += --config-opt=microphone0/dual=true
override config_args += --config-opt=microphone1/dual=true
else
override config_args += --config-opt=microphone0/dual=false
override config_args += --config-opt=microphone1/dual=false
endif
endif

APP_CFLAGS += -O3 -g -DSAMPLING_FREQ_0=$(SAMPLING_FREQ_0) -DSAMPLING_FREQ_1=$(SAMPLING_FREQ_1) -DSIGNAL_FREQ_0_0=$(SIGNAL_FREQ_0_0) -DSIGNAL_FREQ_0_1=$(SIGNAL_FREQ_0_1) -DSIGNAL_FREQ_1_0=$(SIGNAL_FREQ_1_0) -DSIGNAL_FREQ_1_1=$(SIGNAL_FREQ_1_1) -DPDM=$(PDM) -DNB_ITF=$(NB_ITF) -DNB_CHANNELS=$(NB_CHANNELS) -DWORD_SIZE=$(WORD_SIZE) -DNB_ELEM_PER_CHANNEL=$(NB_ELEM) -DNB_CAPTURE=$(NB_CAPTURE) -DNB_START_STOP=$(NB_START_STOP)

gen:
	sox -n -r $(SAMPLING_FREQ_0) --bits $(WORD_SIZE) $(STIM_WAV_0_0) synth 3 sine $(SIGNAL_FREQ_0_0) vol 0.995
	sox -n -r $(SAMPLING_FREQ_0) --bits $(WORD_SIZE) $(STIM_WAV_0_1) synth 3 sine $(SIGNAL_FREQ_0_1) vol 0.995
	sox -n -r $(SAMPLING_FREQ_1) --bits $(WORD_SIZE) $(STIM_WAV_1_0) synth 3 sine $(SIGNAL_FREQ_1_0) vol 0.995
	sox -n -r $(SAMPLING_FREQ_1) --bits $(WORD_SIZE) $(STIM_WAV_1_1) synth 3 sine $(SIGNAL_FREQ_1_1) vol 0.995

include $(RULES_DIR)/pmsis_rules.mk
