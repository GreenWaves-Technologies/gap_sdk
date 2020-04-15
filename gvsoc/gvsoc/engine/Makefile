BUILD_DIR ?= $(CURDIR)/build
ROOT_VP_BUILD_DIR ?= $(BUILD_DIR)
ENGINE_BUILD_DIR = $(BUILD_DIR)/engine

VP_DIRS = vp


-include $(INSTALL_DIR)/rules/vp_models.mk

ifndef VERBOSE
V = @
endif

CC = g++

CFLAGS +=  -MMD -MP -O2 -g -fpic -Isrc -std=c++11 -Werror -Wall -I$(INSTALL_DIR)/include

LDFLAGS += -O2 -g -Werror -Wall -lz -L$(INSTALL_DIR)/lib -Wl,--whole-archive -ljson -Wl,--no-whole-archive

ifdef VP_USE_SYSTEMC
CFLAGS += -D__VP_USE_SYSTEMC -I$(SYSTEMC_HOME)/include
LDFLAGS += -L$(SYSTEMC_HOME)/lib-linux64 -lsystemc
endif

CFLAGS_DBG += -DVP_TRACE_ACTIVE=1
CFLAGS_SV += -DVP_TRACE_ACTIVE=1 -D__VP_USE_SYSTEMV=1

VP_SRCS = src/vp.cpp src/trace/trace.cpp src/clock/clock.cpp src/trace/event.cpp src/trace/vcd.cpp src/trace/lxt2.cpp src/power/power.cpp src/trace/lxt2_write.c src/trace/fst/fastlz.c  src/trace/fst/lz4.c src/trace/fst/fstapi.c src/trace/fst.cpp src/trace/raw.cpp src/trace/raw/trace_dumper.cpp
VP_OBJS = $(patsubst src/%.cpp,$(ENGINE_BUILD_DIR)/%.o,$(patsubst src/%.c,$(ENGINE_BUILD_DIR)/%.o,$(VP_SRCS)))
VP_DBG_OBJS = $(patsubst src/%.cpp,$(ENGINE_BUILD_DIR)/dbg/%.o,$(patsubst src/%.c,$(ENGINE_BUILD_DIR)/dbg/%.o,$(VP_SRCS)))
VP_SV_OBJS = $(patsubst src/%.cpp,$(ENGINE_BUILD_DIR)/sv/%.o,$(patsubst src/%.c,$(ENGINE_BUILD_DIR)/sv/%.o,$(VP_SRCS)))

VP_HEADERS += $(shell find include -name *.hpp)
VP_HEADERS += $(shell find include -name *.h)

VP_HEADERS += $(shell find python -name *.py)



define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	@echo "CP $$<"
	$(V)install -D $(1) $$@

INSTALL_FILES += $(INSTALL_DIR)/$(1)

endef

-include $(VP_OBJS:.o=.d)
-include $(VP_DBG_OBJS:.o=.d)

$(ENGINE_BUILD_DIR)/%.o: src/%.c
	@echo "CXX $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $(CFLAGS) -o $@ -c $<

$(ENGINE_BUILD_DIR)/%.o: src/%.cpp
	@echo "CXX $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $(CFLAGS) -o $@ -c $<

$(ENGINE_BUILD_DIR)/dbg/%.o: src/%.c
	@echo "CXX DBG $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $(CFLAGS) $(CFLAGS_DBG) -o $@ -c $<

$(ENGINE_BUILD_DIR)/dbg/%.o: src/%.cpp
	@echo "CXX DBG $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $(CFLAGS) $(CFLAGS_DBG) -o $@ -c $<

$(ENGINE_BUILD_DIR)/sv/%.o: src/%.c
	@echo "CXX SV $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $(CFLAGS) $(CFLAGS_SV) -o $@ -c $<

$(ENGINE_BUILD_DIR)/sv/%.o: src/%.cpp
	@echo "CXX SV $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $(CFLAGS) $(CFLAGS_SV) -o $@ -c $<

$(ENGINE_BUILD_DIR)/libpulpvp.so: $(VP_OBJS)
	@echo "CXX $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $^ -o $@ $(LDFLAGS) -shared -ldl -lpthread


$(ENGINE_BUILD_DIR)/libpulpvp-debug.so: $(VP_DBG_OBJS)
	@echo "CXX DBG $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $^ -o $@ $(LDFLAGS) -shared  -ldl -lpthread

$(ENGINE_BUILD_DIR)/libpulpvp-sv.so: $(VP_SV_OBJS)
	@echo "CXX SV $<"
	@mkdir -p $(basename $@)
	$(V)$(CC) $^ -o $@ $(LDFLAGS) -shared  -ldl -lpthread

$(ENGINE_BUILD_DIR)/gvsoc_launcher: $(ENGINE_BUILD_DIR)/main.o $(INSTALL_DIR)/lib/libpulpvp.so
	@echo "CXX $<"
	@mkdir -p `dirname $@`
	$(V)$(CC) $(ENGINE_BUILD_DIR)/main.o -o $@ $(LDFLAGS) -lpthread -ldl -lpulpvp

$(ENGINE_BUILD_DIR)/gvsoc_launcher_debug: $(ENGINE_BUILD_DIR)/dbg/main.o $(INSTALL_DIR)/lib/libpulpvp-debug.so
	@echo "CXX DBG $<"
	@mkdir -p `dirname $@`
	$(V)$(CC) $(ENGINE_BUILD_DIR)/dbg/main.o -o $@ $(LDFLAGS) -lpthread -ldl -lpulpvp-debug


$(foreach file, $(VP_HEADERS), $(eval $(call declareInstallFile,$(file))))



$(INSTALL_DIR)/bin/gvsoc_launcher: $(ENGINE_BUILD_DIR)/gvsoc_launcher
	@echo "CP $<"
	$(V)install -D $^ $@

$(INSTALL_DIR)/bin/gvsoc_launcher_debug: $(ENGINE_BUILD_DIR)/gvsoc_launcher_debug
	@echo "CP DBG $<"
	$(V)install -D $^ $@

$(INSTALL_DIR)/lib/libpulpvp.so: $(ENGINE_BUILD_DIR)/libpulpvp.so
	@echo "CP $<"
	$(V)install -D $^ $@

$(INSTALL_DIR)/lib/libpulpvp-debug.so: $(ENGINE_BUILD_DIR)/libpulpvp-debug.so
	@echo "CP DBG $<"
	$(V)install -D $^ $@

$(INSTALL_DIR)/lib/libpulpvp-sv.so: $(ENGINE_BUILD_DIR)/libpulpvp-sv.so
	@echo "CP SV $<"
	$(V)install -D $^ $@

$(INSTALL_DIR)/python/libpulpvp.so: $(ENGINE_BUILD_DIR)/libpulpvp.so
	@echo "CP $<"
	$(V)install -D $^ $@

$(INSTALL_DIR)/python/libpulpvp-debug.so: $(ENGINE_BUILD_DIR)/libpulpvp-debug.so
	@echo "CP DBG $<"
	$(V)install -D $^ $@

$(INSTALL_DIR)/python/libpulpvp-sv.so: $(ENGINE_BUILD_DIR)/libpulpvp-sv.so
	@echo "CP SV $<"
	$(V)install -D $^ $@


headers: $(INSTALL_FILES)

build: $(INSTALL_DIR)/lib/libpulpvp.so $(INSTALL_DIR)/lib/libpulpvp-debug.so $(INSTALL_DIR)/lib/libpulpvp-sv.so $(INSTALL_DIR)/python/libpulpvp.so $(INSTALL_DIR)/python/libpulpvp-debug.so $(INSTALL_DIR)/python/libpulpvp-sv.so $(INSTALL_DIR)/bin/gvsoc_launcher $(INSTALL_DIR)/bin/gvsoc_launcher_debug

clean: vp_clean
	rm -rf $(ENGINE_BUILD_DIR)

.PHONY: build
