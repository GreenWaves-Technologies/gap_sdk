export VP_ROOT_DIR = $(CURDIR)

define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

TARGETS += $(INSTALL_DIR)/$(1)

endef

INSTALL_FILES += bin/gvcontrol
INSTALL_FILES += bin/pulp-pc-info
INSTALL_FILES += bin/pulp-trace-extend
$(foreach file, $(INSTALL_FILES), $(eval $(call declareInstallFile,$(file))))

clean:
	$(MAKE) -C engine clean
	$(MAKE) -C launcher clean
	$(MAKE) -C models clean

build: $(TARGETS)
	install -D vp_models.mk $(INSTALL_DIR)/rules/vp_models.mk
ifdef VP_USE_SYSTEMC_DRAMSYS
	mkdir -p models/memory/dram.sys/build && cd models/memory/dram.sys/build && qmake ../DRAMSys/DRAMSys.pro && make
endif # VP_USE_SYSTEMC_DRAMSYS
	$(MAKE) -C engine headers
	$(MAKE) -C engine build
	$(MAKE) -C launcher build
	$(MAKE) -C dpi-wrapper build
	$(MAKE) -C models props ARCHI_DIR=$(ARCHI_DIR)
	$(MAKE) -C models build ARCHI_DIR=$(ARCHI_DIR)

checkout:
	git submodule update --init
