HEADERS := $(shell find include -name "*.h")

define declareInstallFile

$(TARGET_INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

INSTALL_FILES += $(TARGET_INSTALL_DIR)/$(1)

endef

$(foreach file, $(HEADERS), $(eval $(call declareInstallFile,$(file))))

build: $(INSTALL_FILES)

.PHONY: build
