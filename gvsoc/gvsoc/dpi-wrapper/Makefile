BUILD_DIR ?= build

CFLAGS += -std=gnu++11 -MMD -MP -O3 -g

CFLAGS += -I$(INSTALL_DIR)/include -fPIC
LDFLAGS += -L$(INSTALL_DIR)/lib -fPIC -shared -O3 -g -ljson

DPI_CFLAGS += $(CFLAGS) -DUSE_DPI
DPI_LDFLAGS += $(LDFLAGS) -lpulpvp-sv

DPI_CFLAGS += -Iext/sv/include -Iext/nosv

COMMON_SRCS = 
  
DPI_SRCS = src/dpi.cpp $(COMMON_SRCS)

DPI_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/dpi/%.o,$(patsubst %.c,$(BUILD_DIR)/dpi/%.o,$(DPI_SRCS)))

-include $(DPI_OBJS:.o=.d)

$(BUILD_DIR)/dpi/%.o: %.cpp
	@mkdir -p $(basename $@)
	$(CXX) $(DPI_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/dpi/%.o: %.c
	@mkdir -p $(basename $@)
	$(CXX) $(DPI_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/libgvsocdpi.so: $(DPI_OBJS)
	@mkdir -p $(basename $@)
	$(CXX) -o $@ $^ $(DPI_LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)


$(INSTALL_DIR)/lib/libgvsocdpi.so: $(BUILD_DIR)/libgvsocdpi.so
	install -D $< $@

INSTALL_TARGETS += $(INSTALL_DIR)/lib/libgvsocdpi.so

HEADER_FILES += $(shell find include -name *.hpp)
HEADER_FILES += $(shell find include -name *.h)


define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

INSTALL_HEADERS += $(INSTALL_DIR)/$(1)

endef


$(foreach file, $(HEADER_FILES), $(eval $(call declareInstallFile,$(file))))


build: $(INSTALL_HEADERS) $(INSTALL_TARGETS)

.PHONY: build install
