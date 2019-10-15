BUILD_DIR ?= $(CURDIR)/build
ROOT_VP_BUILD_DIR ?= $(BUILD_DIR)
LAUNCHER_BUILD_DIR = $(ROOT_VP_BUILD_DIR)/launcher


-include $(INSTALL_DIR)/rules/vp_models.mk


CC = g++

CFLAGS +=  -MMD -MP -O2 -g -fpic -std=c++11 -Werror -Wall -I$(INSTALL_DIR)/include
LDFLAGS += -O2 -g -shared -Werror -Wall -L$(INSTALL_DIR)/lib -Wl,--whole-archive -ljson -Wl,--no-whole-archive

LAUNCHER_SRCS = src/launcher.cpp
LAUNCHER_OBJS = $(patsubst src/%.cpp,$(LAUNCHER_BUILD_DIR)/%.o,$(patsubst src/%.c,$(LAUNCHER_BUILD_DIR)/%.o,$(LAUNCHER_SRCS)))

LAUNCHER_HEADERS += $(shell find include -name *.hpp)
LAUNCHER_HEADERS += $(shell find include -name *.h)

define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

INSTALL_FILES += $(INSTALL_DIR)/$(1)

endef

-include $(LAUNCHER_OBJS:.o=.d)

$(LAUNCHER_BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(basename $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(LAUNCHER_BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(basename $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(LAUNCHER_BUILD_DIR)/libpulpvplauncher.so: $(LAUNCHER_OBJS)
	@mkdir -p $(basename $@)
	$(CC) $^ -o $@ $(LDFLAGS)



$(foreach file, $(LAUNCHER_HEADERS), $(eval $(call declareInstallFile,$(file))))



$(INSTALL_DIR)/lib/libpulpvplauncher.so: $(LAUNCHER_BUILD_DIR)/libpulpvplauncher.so
	install -D $^ $@


headers: $(INSTALL_FILES)

build: headers $(INSTALL_DIR)/lib/libpulpvplauncher.so

clean:
	rm -rf $(LAUNCHER_BUILD_DIR)

.PHONY: build
