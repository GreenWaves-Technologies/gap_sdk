APP = test
APP_SRCS = $(GAP_LIB_PATH)/wav_io/wavIO.c test.c 
APP_CFLAGS = -O3 -g -Werror -DWRITE_WAV -I$(GAP_LIB_PATH)/include/

io=host

# To use Freertos
#PMSIS_OS=freertos

clean::
	rm -rf out_file*.wav

include $(RULES_DIR)/pmsis_rules.mk
