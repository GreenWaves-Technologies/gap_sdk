ifdef udma/i2s/version

IMPLEMENTATIONS += devices/sound/i2s_microphone
devices/sound/i2s_microphone_SRCS = devices/sound/i2s_microphone.cpp

IMPLEMENTATIONS += devices/sound/i2s_speaker
devices/sound/i2s_speaker_SRCS = devices/sound/i2s_speaker.cpp

sndfile=$(shell pkg-config --exists sndfile || echo FAILED)

ifeq '$(sndfile)' ''
USE_SNDFILE=1
devices/sound/i2s_microphone_CFLAGS = -DUSE_SNDFILE
devices/sound/i2s_microphone_LDFLAGS = -lsndfile
devices/sound/i2s_speaker_CFLAGS = -DUSE_SNDFILE
devices/sound/i2s_speaker_LDFLAGS = -lsndfile
endif

endif