ifeq '$(pulp_chip_family)' 'gap'

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/gap/rom.bin

$(INSTALL_DIR)/python/pulp/chips/gap/rom.bin: $(CURDIR)/pulp/chips/gap/rom.bin
	install -D $^ $@

IMPLEMENTATIONS += pulp/chips/gap/apb_soc_impl
pulp/chips/gap/apb_soc_impl_SRCS = pulp/chips/gap/apb_soc_impl.cpp

endif
