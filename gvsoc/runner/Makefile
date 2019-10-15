WORKSTATION_PKG ?= $(INSTALL_DIR)

HEADER_FILES += $(shell find python -name "*.py")
HEADER_FILES += python/runner/fpga/pulpissimo/elf_run_genesys2.sh
HEADER_FILES += python/runner/fpga/pulpissimo/elf_run.gdb
HEADER_FILES += python/runner/fpga/pulpissimo/openocd-genesys2.cfg


define declareInstallFile
$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@
INSTALL_HEADERS += $(INSTALL_DIR)/$(1)
endef


$(foreach file, $(HEADER_FILES), $(eval $(call declareInstallFile,$(file))))


sdk.clean:

sdk.header:

sdk.build: $(INSTALL_HEADERS)
	install -d $(WORKSTATION_PKG)/bin
	install -d $(WORKSTATION_PKG)/ref
	install -D bin/* $(WORKSTATION_PKG)/bin
	-gcc -O3 -o $(WORKSTATION_PKG)/bin/aes_encode aes/AesLib.c aes/main.c
