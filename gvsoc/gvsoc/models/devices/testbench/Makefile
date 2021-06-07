IMPLEMENTATIONS += devices/testbench/testbench
devices/testbench/testbench_SRCS = devices/testbench/testbench.cpp devices/testbench/spim_verif.cpp devices/testbench/i2s_verif.cpp

sndfile=$(shell pkg-config --exists sndfile || echo FAILED)

ifeq '$(sndfile)' ''
USE_SNDFILE=1
devices/testbench/testbench_CFLAGS = -DUSE_SNDFILE
devices/testbench/testbench_LDFLAGS = -lsndfile
endif
