APP = test
APP_SRCS = test.c
APP_CFLAGS = -O3 -g
APP_LDFLAGS = -lgcc

CONFIG_I2S=1

SAMPLING_FREQ = 44100
WORD_SIZE = 16
SIGNAL_FREQ = 4000
STIM_WAV_0_0 ?= $(CONFIG_BUILD_DIR)/stim_0_0.wav

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

override config_args += --config-opt=board/components/mic0/mic/stim=$(STIM_WAV_0_0)
override config_args += --config-opt=board/components/mic0/mic/stim_incr_start=0x0055
override config_args += --config-opt=board/components/mic0/mic/stim_mode=incr

override config_args += --config-opt=board/components/mic1/mic/stim=$(STIM_WAV_0_0)
override config_args += --config-opt=board/components/mic1/mic/stim_incr_start=0x1055
override config_args += --config-opt=board/components/mic1/mic/stim_mode=incr

override config_args += --config-opt=board/components/mic2/mic/stim=$(STIM_WAV_0_0)
override config_args += --config-opt=board/components/mic2/mic/stim_incr_start=0x2055
override config_args += --config-opt=board/components/mic2/mic/stim_mode=incr

override config_args += --config-opt=board/components/mic3/mic/stim=$(STIM_WAV_0_0)
override config_args += --config-opt=board/components/mic3/mic/stim_incr_start=0x3055
override config_args += --config-opt=board/components/mic3/mic/stim_mode=incr


gen:
	sox -n -r $(SAMPLING_FREQ) --bits $(WORD_SIZE) $(STIM_WAV_0_0) synth 3 sine $(SIGNAL_FREQ) vol 0.995


include $(RULES_DIR)/pmsis_rules.mk
