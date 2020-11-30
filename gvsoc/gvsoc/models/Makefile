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


VP_DIRS=memory cpu/iss board \
  interco utils \
  devices/hyperbus devices/spiflash  \
  cache devices/uart devices/sound \
  devices/testbench devices/camera devices/jtag


-include $(ROOT_VP_BUILD_DIR)/props.mk

-include $(INSTALL_DIR)/rules/vp_models.mk

props:
	plpinfo mkgen --makefile=$(ROOT_VP_BUILD_DIR)/props.mk $(properties)

clean: vp_clean

.PHONY: clean build props
