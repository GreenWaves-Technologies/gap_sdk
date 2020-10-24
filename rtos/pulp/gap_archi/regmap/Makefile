TARGET_INSTALL_DIR ?= $(CURDIR)/install
INSTALL_DIR ?= $(CURDIR)/install
BUILD_DIR   ?= $(CURDIR)/build

all: header build

define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

INSTALL_HEADERS += $(INSTALL_DIR)/$(1)

endef

define declarePyInstallFile

$(INSTALL_DIR)/python/$(1): bin/$(1)
	install -D bin/$(1) $$@

PY_INSTALL_HEADERS += $(INSTALL_DIR)/python/$(1)

endef


INSTALL_FILES += bin/regmap
PY_INSTALL_FILES += $(shell cd bin && find -name "*.py")

$(foreach file, $(INSTALL_FILES), $(eval $(call declareInstallFile,$(file))))

$(foreach file, $(PY_INSTALL_FILES), $(eval $(call declarePyInstallFile,$(file))))

build: $(INSTALL_HEADERS) $(PY_INSTALL_HEADERS)