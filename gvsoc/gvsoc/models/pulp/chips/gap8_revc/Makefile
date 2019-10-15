ifeq '$(pulp_chip)' 'gap8_revc'

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/gap8_revc/rom.bin

$(INSTALL_DIR)/python/pulp/chips/gap8_revc/rom.bin: $(CURDIR)/pulp/chips/gap8_revc/rom.bin
	install -D $^ $@

endif