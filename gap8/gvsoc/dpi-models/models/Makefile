BUILD_DIR ?= $(CURDIR)/build

ROOT_DPI_BUILD_DIR ?= $(BUILD_DIR)

DPI_DIRS=test/spim_verif jtag/proxy uart/uart microphone eeprom lcd wifi ram/spiram flash/spiflash camera 

-include $(INSTALL_DIR)/rules/dpi_rules.mk


build: dpi_build

clean: dpi_clean

.PHONY: clean build
