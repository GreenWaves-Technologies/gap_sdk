VSIM_DETECTED_PATH=$(dir $(shell which vsim))
BUILD_DIR ?= build

CFLAGS += -std=gnu++11 -MMD -MP -O3 -g

CFLAGS += -I$(INSTALL_DIR)/include -fPIC
LDFLAGS += -L$(INSTALL_DIR)/lib -fPIC -shared -O3 -g -ljson

DPI_CFLAGS += $(CFLAGS) -DUSE_DPI
DPI_LDFLAGS += $(LDFLAGS)  -Wl,-export-dynamic -ldl -rdynamic -lpulpperiph

ifdef VSIM_INCLUDE
DPI_CFLAGS += -Iext/sv/include -I$(VSIM_INCLUDE) -DVSIM_INCLUDE=1
else
DPI_CFLAGS += -Iext/sv/include -Iext/nosv
endif

PERIPH_CFLAGS += $(CFLAGS) $(DPI_CFLAGS)
PERIPH_LDFLAGS += $(LDFLAGS)  -Wl,-export-dynamic -ldl -rdynamic

COMMON_SRCS = src/qspim.cpp src/gpio.cpp src/jtag.cpp src/ctrl.cpp \
  src/uart.cpp src/cpi.cpp src/i2s.cpp src/i2c.cpp src/telnet_proxy.cpp
  
DPI_SRCS = src/dpi.cpp $(COMMON_SRCS)
PERIPH_SRCS = src/models.cpp $(COMMON_SRCS)

DPI_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/dpi/%.o,$(patsubst %.c,$(BUILD_DIR)/dpi/%.o,$(DPI_SRCS)))
PERIPH_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/periph/%.o,$(patsubst %.c,$(BUILD_DIR)/periph/%.o,$(PERIPH_SRCS)))

-include $(DPI_OBJS:.o=.d)
-include $(PERIPH_OBJS:.o=.d)

$(BUILD_DIR)/dpi/%.o: %.cpp
	@mkdir -p $(basename $@)
	$(CXX) $(DPI_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/dpi/%.o: %.c
	@mkdir -p $(basename $@)
	$(CXX) $(DPI_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/periph/%.o: %.cpp
	@mkdir -p $(basename $@)
	$(CXX) $(PERIPH_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/periph/%.o: %.c
	@mkdir -p $(basename $@)
	$(CXX) $(PERIPH_CFLAGS) -o $@ -c $<

$(BUILD_DIR)/libpulpdpi.so: $(DPI_OBJS)
	@mkdir -p $(basename $@)
	$(CXX) -o $@ $^ $(DPI_LDFLAGS)

$(BUILD_DIR)/libpulpperiph.so: $(PERIPH_OBJS)
	@mkdir -p $(basename $@)
	$(CXX) -o $@ $^ $(PERIPH_LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)
	make -C models clean


$(INSTALL_DIR)/lib/libpulpdpi.so: $(BUILD_DIR)/libpulpdpi.so
	install -D $< $@

$(INSTALL_DIR)/lib/libpulpperiph.so: $(BUILD_DIR)/libpulpperiph.so
	install -D $< $@

$(INSTALL_DIR)/rules/dpi_rules.mk: dpi_rules.mk
	install -D $< $@


INSTALL_TARGETS += $(INSTALL_DIR)/lib/libpulpperiph.so
INSTALL_TARGETS += $(INSTALL_DIR)/lib/libpulpdpi.so

HEADER_FILES += $(shell find include -name *.hpp)
HEADER_FILES += $(shell find include -name *.h)


define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

INSTALL_HEADERS += $(INSTALL_DIR)/$(1)

endef


$(foreach file, $(HEADER_FILES), $(eval $(call declareInstallFile,$(file))))


build: $(INSTALL_HEADERS) $(INSTALL_TARGETS) $(INSTALL_DIR)/rules/dpi_rules.mk
	make -C models build


checkout:
	git submodule update --init

.PHONY: checkout build install
