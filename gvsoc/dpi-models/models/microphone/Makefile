DPI_MODELS += i2s_microphone

i2s_microphone_SRCS = microphone/i2s_microphone.cpp

sndfile=$(shell pkg-config --exists sndfile || echo FAILED)

ifeq '$(sndfile)' ''
USE_SNDFILE=1
i2s_microphone_LDFLAGS += -lsndfile
i2s_microphone_CFLAGS += -DUSE_SNDFILE
endif
