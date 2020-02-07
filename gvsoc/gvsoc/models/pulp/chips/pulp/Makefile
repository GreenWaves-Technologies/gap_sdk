ifeq '$(pulp_chip)' 'pulp'
IMPLEMENTATIONS += pulp/chips/pulp/apb_soc_impl
pulp/chips/pulp/apb_soc_impl_SRCS = pulp/chips/pulp/apb_soc_impl.cpp

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/pulp/rom.bin
VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/pulp/debug_rom.bin

$(INSTALL_DIR)/python/pulp/chips/pulp/rom.bin: $(CURDIR)/pulp/chips/pulp/rom.bin
	install -D $^ $@

$(INSTALL_DIR)/python/pulp/chips/pulp/debug_rom.bin: $(CURDIR)/pulp/chips/pulp/debug_rom.bin
	install -D $^ $@

endif
