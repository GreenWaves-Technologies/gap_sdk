ifeq '$(pulp_chip)' 'vivosoc3_1'

IMPLEMENTATIONS += pulp/chips/vivosoc3_1/apb_soc_impl
pulp/chips/vivosoc3_1/apb_soc_impl_SRCS = pulp/chips/vivosoc3_1/apb_soc_impl.cpp

VP_INSTALL_TARGETS += $(INSTALL_DIR)/python/pulp/chips/vivosoc3_1/rom.bin

$(INSTALL_DIR)/python/pulp/chips/vivosoc3_1/rom.bin: $(CURDIR)/pulp/chips/vivosoc3_1/rom.bin
	install -D $^ $@

endif
