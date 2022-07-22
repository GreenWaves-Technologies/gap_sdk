
define include_src

-include $(1)/rules/pulpos/src.mk

endef

$(foreach module, $(PULPOS_MODULES), $(eval $(call include_src,$(module))))

