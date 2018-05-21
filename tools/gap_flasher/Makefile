ifndef GAP_SDK_HOME
    $(error Please run 'source sourceme.sh' in gap_sdk first)
endif

FLASHER_SRC = $(CURDIR)/src
FLASHER_TOOL = $(CURDIR)/tool

BIN_FILES = $(shell find bin -type f)

define declareInstallFile
$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@
INSTALL_HEADERS += $(INSTALL_DIR)/$(1)
endef

$(foreach file, $(BIN_FILES), $(eval $(call declareInstallFile,$(file))))

all:: install_flasher install_files

build_flasher:
	cd $(FLASHER_SRC) && make clean all

install_flasher: build_flasher
	install -D $(FLASHER_SRC)/BUILD/GAP8/GCC_RISCV/test $(INSTALL_DIR)/bin/flasher

install_files: $(INSTALL_HEADERS)

clean::
	cd $(FLASHER_SRC) && make clean
