APP = test
APP_SRCS = tx_only.c
APP_CFLAGS = -O3 -g
APP_LDFLAGS = -lgcc

CONFIG_I2S=1

SAMPLING_FREQ = 44100
WORD_SIZE = 32
SIGNAL_FREQ = 4000

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

override config_args += --config-opt=board/components/sink0/sink/outfile=outfile_0.hex


include $(RULES_DIR)/pmsis_rules.mk
