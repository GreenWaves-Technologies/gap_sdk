TARGET_INSTALL_DIR ?= $(CURDIR)/install
INSTALL_DIR ?= $(CURDIR)/install
BUILD_DIR   ?= $(CURDIR)/build

all: header build

export PYTHONPATH:=$(CURDIR)/bin:$(PYTHONPATH)

define declarePyInstallFile

$(INSTALL_DIR)/python/$(1): bin/$(1)
	install -D bin/$(1) $$@

PY_INSTALL_HEADERS += $(INSTALL_DIR)/python/$(1)

endef

define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

INSTALL_HEADERS += $(INSTALL_DIR)/$(1)

endef


PY_INSTALL_FILES += $(shell cd bin && find -name "*.py")
INSTALL_FILES += bin/plptest

$(INSTALL_DIR)/bin/plptest_checker: $(BUILD_DIR)/plptest_checker
	install -D $< $@

$(BUILD_DIR)/plptest_checker: src/plptest_checker.c
	mkdir -p $(BUILD_DIR)
	gcc -O3 -g src/plptest_checker.c -o $(BUILD_DIR)/plptest_checker

$(foreach file, $(INSTALL_FILES), $(eval $(call declareInstallFile,$(file))))

$(foreach file, $(PY_INSTALL_FILES), $(eval $(call declarePyInstallFile,$(file))))

build: $(INSTALL_DIR)/bin/plptest_checker $(INSTALL_HEADERS) $(PY_INSTALL_HEADERS)
