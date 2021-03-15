APP = test
APP_SRCS = rx_only.c wav_out.c
APP_CFLAGS = -O3 -g #-DTRACE_I2S
APP_LDFLAGS = -lgcc

CONFIG_I2S=1

SAMPLING_FREQ = 44100
WORD_SIZE = 32
SIGNAL_FREQ = 4000

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

override config_args += --config-opt=board/components/mic0/mic/enabled=true
override config_args += --config-opt=board/components/mic0/mic/width=$(WORD_SIZE)
override config_args += --config-opt=board/components/mic0/mic/stim_value=0
override config_args += --config-opt=board/components/mic0/mic/stim_incr_start=0x0055
override config_args += --config-opt=board/components/mic0/mic/stim_mode=incr

override config_args += --config-opt=board/components/mic1/mic/enabled=true
override config_args += --config-opt=board/components/mic1/mic/width=$(WORD_SIZE)
override config_args += --config-opt=board/components/mic1/mic/stim_value=1
override config_args += --config-opt=board/components/mic1/mic/stim_incr_start=0x1055
override config_args += --config-opt=board/components/mic1/mic/stim_mode=incr

override config_args += --config-opt=board/components/mic2/mic/enabled=true
override config_args += --config-opt=board/components/mic2/mic/width=$(WORD_SIZE)
override config_args += --config-opt=board/components/mic2/mic/stim_value=2
override config_args += --config-opt=board/components/mic2/mic/stim_incr_start=0x2055
override config_args += --config-opt=board/components/mic2/mic/stim_mode=incr

override config_args += --config-opt=board/components/mic3/mic/enabled=true
override config_args += --config-opt=board/components/mic3/mic/width=$(WORD_SIZE)
override config_args += --config-opt=board/components/mic3/mic/stim_value=3
override config_args += --config-opt=board/components/mic3/mic/stim_incr_start=0x3055
override config_args += --config-opt=board/components/mic3/mic/stim_mode=incr


include $(RULES_DIR)/pmsis_rules.mk
