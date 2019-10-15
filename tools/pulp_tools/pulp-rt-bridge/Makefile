INSTALL_DIR ?= $(CURDIR)/install
TARGET_INSTALL_DIR ?= $(CURDIR)/install
BUILD_DIR   ?= $(CURDIR)/build



SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

ifneq '$(SDL_CFLAGS)$(SDL_LDFLAGS)' ''
USE_SDL=1
endif


HEADER_FILES += $(shell find python -name *.py)
HEADER_FILES += $(shell find bin -type f)

define declareInstallFile
$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@
INSTALL_HEADERS += $(INSTALL_DIR)/$(1)
endef


CFLAGS += -O3 -g -fPIC -std=gnu++11 -MMD -MP -Isrc -Iinclude -I$(INSTALL_DIR)/include $(SDL_CFLAGS)
LDFLAGS += -O3 -g -shared $(SDL_LDFLAGS)

SRCS = src/python_wrapper.cpp src/reqloop.cpp

LDFLAGS += -L$(INSTALL_DIR)/lib
LDFLAGS += -ljson

ifneq '$(USE_SDL)' ''
CFLAGS += -D__USE_SDL__
endif

OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

$(foreach file, $(HEADER_FILES), $(eval $(call declareInstallFile,$(file))))

all: build

install:

-include $(OBJS:.o=.d)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(basename $@)
	g++ $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/librtbridge.so: $(OBJS)
	g++ -o $@ $^ $(LDFLAGS)

$(INSTALL_DIR)/lib/librtbridge.so: $(BUILD_DIR)/librtbridge.so
	install -D $< $@

build: $(INSTALL_HEADERS) $(INSTALL_DIR)/lib/librtbridge.so

clean:
	rm -rf $(BUILD_DIR)
