ifndef GAP_SDK_HOME
PULP_PROPERTIES += udma/cpi/version pulp_chip_family
include $(TARGET_INSTALL_DIR)/rules/pulp_properties.mk
endif

SRC = \
  frame_streamer/frame_streamer.c


SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

CV_CFLAGS = $(shell pkg-config opencv --cflags)
CV_LDFLAGS = $(shell pkg-config opencv --libs)

ifneq '$(SDL_CFLAGS)$(SDL_LDFLAGS)' ''
USE_SDL=1
SDL_CFLAGS += -D__USE_SDL__
endif

ifneq '$(SDL_CFLAGS)$(SDL_LDFLAGS)' ''
USE_CV=1
CV_CFLAGS += -D__USE_OPENCV__
endif

WS_CFLAGS += -std=c++11 -O2 -g -fPIC -MMD -MP $(SDL_CFLAGS) $(CV_CFLAGS) -I$(INSTALL_DIR)/include -I$(CURDIR)/include
WS_LDFLAGS += -O2 -g -shared $(SDL_LDFLAGS) $(CV_LDFLAGS)
WS_SRCS += python/pmsis_tools.cpp python/pmsis_tools_frame_streamer.cpp python/pmsis_tools_transport_socket.cpp
WS_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(WS_SRCS))

-include $(WS_OBJS:.o=.d)

define declareInstallFile
$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@
INSTALL_HEADERS += $(INSTALL_DIR)/$(1)
endef

-include $(WS_OBJS:.o=.d)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(basename $@)
	g++ $(WS_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/libpmsis_tools.so: $(WS_OBJS)
	g++ -o $@ $^ $(WS_LDFLAGS)

$(INSTALL_DIR)/lib/libpmsis_tools.so: $(BUILD_DIR)/libpmsis_tools.so
	install -D $< $@

HEADER_FILES += $(shell find python -name *.py)
HEADER_FILES += include/tools/services.h

$(foreach file, $(HEADER_FILES), $(eval $(call declareInstallFile,$(file))))

header: header_target

build: $(INSTALL_HEADERS) 	$(INSTALL_DIR)/lib/libpmsis_tools.so all_target

all: header build

clean: clean_target
	rm -rf $(BUILD_DIR)

ifdef GAP_SDK_HOME
include $(CURDIR)/rules/gap_sdk.mk
else
include $(CURDIR)/rules/pulp_sdk.mk
endif
