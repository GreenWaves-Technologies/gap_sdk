BUILD_DIR ?= $(CURDIR)/build

CC = g++

CFLAGS +=  -MMD -MP -O2 -g -std=c++11 -Werror -Wall -I$(INSTALL_DIR)/include
LDFLAGS += -O2 -g -Werror -Wall -L$(INSTALL_DIR)/lib -lpulpvp-debug

SRCS = launcher.cpp
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: $(BUILD_DIR)/launcher

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/launcher: $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: build clean
