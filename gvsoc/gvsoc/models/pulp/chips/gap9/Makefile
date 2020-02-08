ifeq '$(pulp_chip)' 'gap9'

IMPLEMENTATIONS += pulp/chips/gap9/apb_soc_impl
pulp/chips/gap9/apb_soc_impl_SRCS = pulp/chips/gap9/apb_soc_impl.cpp

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/gap9/rom.bin
VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/gap9/debug_rom.bin

$(INSTALL_DIR)/python/pulp/chips/gap9/rom.bin: $(CURDIR)/pulp/chips/gap9/rom.bin
	install -D $^ $@

$(INSTALL_DIR)/python/pulp/chips/gap9/debug_rom.bin: $(CURDIR)/pulp/chips/gap9/debug_rom.bin
	install -D $^ $@

endif
