export VP_ROOT_DIR = $(CURDIR)

define declareInstallFile

$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@

TARGETS += $(INSTALL_DIR)/$(1)

endef

GV_MAKEFLAGS ?= -j 4

INSTALL_FILES += bin/pulp-pc-info
INSTALL_FILES += bin/pulp-trace-extend
$(foreach file, $(INSTALL_FILES), $(eval $(call declareInstallFile,$(file))))

clean:
	make -C engine clean
	make -C launcher clean
	make -C models clean

build: $(TARGETS)
	install -D vp_models.mk $(INSTALL_DIR)/rules/vp_models.mk
ifdef VP_USE_SYSTEMC_DRAMSYS
	mkdir -p models/memory/dram.sys/build && cd models/memory/dram.sys/build && qmake ../DRAMSys/DRAMSys.pro && make
endif # VP_USE_SYSTEMC_DRAMSYS
	make -C engine headers
	make -C engine build $(GV_MAKEFLAGS)
	make -C launcher build
	make -C dpi-wrapper build
	make -C models props ARCHI_DIR=$(ARCHI_DIR)
	make -C models build ARCHI_DIR=$(ARCHI_DIR) $(GV_MAKEFLAGS)

checkout:
	git submodule update --init
