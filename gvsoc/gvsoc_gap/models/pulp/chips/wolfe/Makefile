ifeq '$(pulp_chip_family)' 'wolfe'

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/wolfe/rom.bin

$(INSTALL_DIR)/python/pulp/chips/wolfe/rom.bin: $(CURDIR)/pulp/chips/wolfe/rom.bin
	install -D $^ $@


IMPLEMENTATIONS += pulp/chips/wolfe/apb_soc_impl
pulp/chips/wolfe/apb_soc_impl_SRCS = pulp/chips/wolfe/apb_soc_impl.cpp

endif
