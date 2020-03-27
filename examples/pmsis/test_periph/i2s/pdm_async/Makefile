APP = test
APP_SRCS = test.c
APP_CFLAGS = -O3 -g -Werror
APP_LDFLAGS = -lgcc

RUNNER_CONFIG = $(CURDIR)/config.ini

# Configure GVSOC microphone model for PDM
override config_args += --config-opt=microphone0/stim_left=$(BUILDDIR)/stim_0_0.wav
override config_args += --config-opt=microphone0/pdm=true

gen:
	sox -n -r 44100 --bits 16 $(BUILDDIR)/stim_0_0.wav synth 3 sine 800 vol 0.995

include $(RULES_DIR)/pmsis_rules.mk
