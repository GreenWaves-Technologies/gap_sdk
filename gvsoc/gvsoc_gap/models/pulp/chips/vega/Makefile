ifeq '$(pulp_chip)' 'vega'

IMPLEMENTATIONS += pulp/chips/vega/apb_soc_impl
pulp/chips/vega/apb_soc_impl_SRCS = pulp/chips/vega/apb_soc_impl.cpp

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/vega/rom.bin
VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/vega/debug_rom.bin

$(INSTALL_DIR)/python/pulp/chips/vega/rom.bin: $(CURDIR)/pulp/chips/vega/rom.bin
	install -D $^ $@

$(INSTALL_DIR)/python/pulp/chips/vega/debug_rom.bin: $(CURDIR)/pulp/chips/vega/debug_rom.bin
	install -D $^ $@

endif
