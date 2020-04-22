ifdef udma/i2s/version

IMPLEMENTATIONS += devices/microphone/i2s_microphone
devices/microphone/i2s_microphone_SRCS = devices/microphone/i2s_microphone.cpp

sndfile=$(shell pkg-config --exists sndfile || echo FAILED)

ifeq '$(sndfile)' ''
USE_SNDFILE=1
devices/microphone/i2s_microphone_CFLAGS = -DUSE_SNDFILE
devices/microphone/i2s_microphone_LDFLAGS = -lsndfile
endif

endif