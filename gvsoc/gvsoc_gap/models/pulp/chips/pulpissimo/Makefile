ifeq '$(pulp_chip)' 'pulpissimo'

IMPLEMENTATIONS += pulp/chips/pulpissimo/apb_soc_impl
pulp/chips/pulpissimo/apb_soc_impl_SRCS = pulp/chips/pulpissimo/apb_soc_impl.cpp

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/pulpissimo/rom.bin
VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/pulpissimo/debug_rom.bin

$(INSTALL_DIR)/python/pulp/chips/pulpissimo/rom.bin: $(CURDIR)/pulp/chips/pulpissimo/rom.bin
	install -D $^ $@

$(INSTALL_DIR)/python/pulp/chips/pulpissimo/debug_rom.bin: $(CURDIR)/pulp/chips/pulpissimo/debug_rom.bin
	install -D $^ $@

endif