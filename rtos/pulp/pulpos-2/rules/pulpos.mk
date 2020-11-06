-include $(PULPOS_HOME)/rules/pulpos/targets/$(PULPOS_TARGET).mk

define include_target

-include $(1)/rules/pulpos/targets/$(PULPOS_TARGET).mk

endef

$(foreach module, $(PULPOS_MODULES), $(eval $(call include_target,$(module))))
