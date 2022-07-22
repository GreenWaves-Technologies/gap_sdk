APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

# Activate proxy mode
GV_PROXY = 1
GV_PROXY_PORT ?= 30000

# Activate GVSOC debug mode so that traces can be enabled dynamically
GV_DEBUG_MODE = 1

CONFIG_TESTBENCH = 1
CONFIG_I2S = 1

# This target should be launched from another terminal and will connect to gvsoc
# to launch a few commands
proxy:
	./gvcontrol --host=localhost --port=$(GV_PROXY_PORT)

run_dual:
	./launch.py --run="$(RUN_CMD)" --proxy="$(PROXY_CMD)"

gen:
	sox -n -r 44100 --bits 16 sound_0.wav synth 3 sine 1000 vol 0.995
	sox -n -r 44100 --bits 16 sound_1.wav synth 3 sine 2000 vol 0.995

include $(RULES_DIR)/pmsis_rules.mk