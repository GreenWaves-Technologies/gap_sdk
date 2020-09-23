ifeq '$(pulp_chip)' 'gap9_v2'

IMPLEMENTATIONS += pulp/chips/gap9_v2/apb_soc_impl
pulp/chips/gap9_v2/apb_soc_impl_SRCS = pulp/chips/gap9_v2/apb_soc_impl.cpp

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/gap9_v2/rom.bin
VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/gap9_v2/debug_rom.bin

$(INSTALL_DIR)/python/pulp/chips/gap9_v2/rom.bin: $(CURDIR)/pulp/chips/gap9_v2/rom.bin
	install -D $^ $@

$(INSTALL_DIR)/python/pulp/chips/gap9_v2/debug_rom.bin: $(CURDIR)/pulp/chips/gap9_v2/debug_rom.bin
	install -D $^ $@

endif
