BUILD_DIR ?= $(CURDIR)/build
ROOT_VP_BUILD_DIR ?= $(BUILD_DIR)

PULP_PROPERTIES += udma/version timer/version stdout/version soc_eu/version \
	fc_itc/version icache_ctrl/version xne/version fll/version event_unit/version \
	cluster_ctrl/version pulp_chip cluster/version dma/version udma/spim/version \
	udma/cpi/version padframe/version pulp_chip_family pmu/version \
	udma/hyper/version efuse/version udma/i2c/version rtc/version gpio/version \
  fc/iss_class pe/iss_class udma/i2s/version udma/mram/version hwce/version \
   udma/tcdm/version hwacc/base udma/uart/version

properties := $(foreach prop,$(PULP_PROPERTIES), --property=$(prop))


VP_DIRS=memory pulp pulp/fll pulp/stdout pulp/chips/pulpissimo cpu/iss \
  interco utils pulp/itc pulp/hwpe/example pulp/timer pulp/soc_eu pulp/udma \
  pulp/hwpe/xne pulp/icache_ctrl pulp/cluster pulp/event_unit \
  pulp/chips/pulp pulp/mchan pulp/padframe pulp/adv_dbg_unit \
  pulp/chips/oprecompkw pulp/chips/oprecompkw_sa pulp/chips/bigpulp \
  pulp/chips/wolfe pulp/chips/vega pulp/chips/gap9 pulp/chips/gap9_v2 pulp/chips/usoc_v1 pulp/pmu pulp/chips/gap \
  pulp/chips/multino pulp/efuse board pulp/chips/arnold \
  devices/hyperbus devices/spiflash vendor/dolphin pulp/chips/pulpissimo_v1 \
  pulp/rtc pulp/gpio pulp/chips/gap_rev1 pulp/chips/pulp_v1 pulp/chips/vivosoc3_1 \
  pulp/mram pulp/hwce cache pulp/chips/gap8_revc pulp/hwacc devices/uart devices/sound \
  devices/testbench


-include $(ROOT_VP_BUILD_DIR)/props.mk

-include $(INSTALL_DIR)/rules/vp_models.mk

props:
	plpinfo mkgen --makefile=$(ROOT_VP_BUILD_DIR)/props.mk $(properties)

clean: vp_clean

.PHONY: clean build props
