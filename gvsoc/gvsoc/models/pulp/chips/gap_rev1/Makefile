ifeq '$(pulp_chip)' 'gap_rev1'

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/gap_rev1/rom.bin

$(INSTALL_DIR)/python/pulp/chips/gap_rev1/rom.bin: $(CURDIR)/pulp/chips/gap_rev1/rom.bin
	install -D $^ $@

endif