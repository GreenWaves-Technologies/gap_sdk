APP = test
APP_SRCS = test.c wav_out.c
APP_CFLAGS = -O3 -g -Werror
APP_LDFLAGS = -lgcc

RUNNER_CONFIG = $(CURDIR)/config.ini

io=host

# Configure GVSOC microphone model for PDM on 4 microphones using double-data rate on the 2 interfaces
override config_args += --config-opt=microphone0/stim_left=$(BUILDDIR)/stim_0_0.wav
override config_args += --config-opt=microphone0/stim_right=$(BUILDDIR)/stim_0_1.wav
override config_args += --config-opt=microphone0/pdm=true
override config_args += --config-opt=microphone0/ddr=true
override config_args += --config-opt=microphone1/stim_left=$(BUILDDIR)/stim_1_0.wav
override config_args += --config-opt=microphone1/stim_right=$(BUILDDIR)/stim_1_1.wav
override config_args += --config-opt=microphone1/pdm=true
override config_args += --config-opt=microphone1/ddr=true

gen:
	sox -n -r 44100 --bits 16 $(BUILDDIR)/stim_0_0.wav synth 3 sine 200 vol 0.995
	sox -n -r 44100 --bits 16 $(BUILDDIR)/stim_0_1.wav synth 3 sine 400 vol 0.995
	sox -n -r 44100 --bits 16 $(BUILDDIR)/stim_1_0.wav synth 3 sine 800 vol 0.995
	sox -n -r 44100 --bits 16 $(BUILDDIR)/stim_1_1.wav synth 3 sine 1600 vol 0.995
include $(RULES_DIR)/pmsis_rules.mk
