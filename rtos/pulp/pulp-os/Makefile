PULP_LIBS = rt

ifdef MK_ROOT

include $(MK_ROOT)/props.mk

BUILD_DIR          ?= $(CURDIR)/build
CONFIG_BUILD_DIR   ?= $(subst =,.,$(BUILD_DIR)/$(pulp_chip))

else

PULP_PROPERTIES += fc/archi pe/archi pulp_chip pulp_chip_family cluster/version
PULP_PROPERTIES += host/archi fc_itc/version udma/hyper/version udma/cpi/version udma/i2c/version soc/fll/version
PULP_PROPERTIES += udma/i2s/version udma/uart/version event_unit/version perf_counters
PULP_PROPERTIES += fll/version soc/spi_master soc/apb_uart padframe/version
PULP_PROPERTIES += udma/spim/version gpio/version udma/archi udma/version
PULP_PROPERTIES += soc_eu/version compiler rtc/version udma/mram/version hwce/version
PULP_PROPERTIES += tcdm/version

include $(TARGET_INSTALL_DIR)/rules/pulp_properties.mk

endif

ifndef PULP_RT_CONFIG
PULP_RT_CONFIG = configs/pulpos.mk
endif

include $(PULP_RT_CONFIG)

ifdef ARCHI_DIR
PULP_CFLAGS += -I$(ARCHI_DIR)
endif

PULP_CFLAGS += -I$(CURDIR)/kernel

ifdef USE_PMSIS
PULP_CFLAGS += -D__RT_USE_PMSIS__=1
endif

ifdef CONFIG_IO_ENABLED
PULP_CFLAGS += -D__RT_USE_IO=1
endif

ifdef CONFIG_ASSERT_ENABLED
PULP_CFLAGS += -D__RT_USE_ASSERT=1
endif

ifdef CONFIG_TRACE_ENABLED
PULP_CFLAGS += -D__RT_USE_TRACE=1
endif

ifdef CONFIG_PROFILE_ENABLED
PULP_CFLAGS += -D__RT_USE_PROFILE=1
endif

ifdef CONFIG_CFLAGS
PULP_CFLAGS += $(CONFIG_CFLAGS)
endif

PULP_CFLAGS += -Os -g -fno-jump-tables -Werror
PULP_CFLAGS += -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -Wno-address-of-packed-member
ifneq '$(compiler)' 'llvm'
PULP_CFLAGS += -fno-tree-loop-distribute-patterns
endif

INSTALL_FILES += $(shell find include -name *.h)
INSTALL_FILES += $(shell find rules -name *.ld)
WS_INSTALL_FILES += include/rt/data/rt_data_bridge.h





ifndef MK_ROOT
HAL_FILES := $(shell plpfiles copy --item=hal_src_files)
PULP_LIB_FC_SRCS_rt += $(HAL_FILES)
endif


include kernel/kernel.mk
include drivers/drivers.mk
include drivers_deprecated/drivers.mk
include libs/libs.mk


ifndef MK_ROOT
include $(TARGET_INSTALL_DIR)/rules/pulp_rt.mk
else
include $(MK_ROOT)/config.mk
endif


define halSrcRules

$(CONFIG_BUILD_DIR)/fc/$(1): $(TARGET_INSTALL_DIR)/src/$(2)
	@mkdir -p `dirname $$@`
	$(PULP_FC_CC) $(rt_cl_cflags) -MMD -MP -c $$< -o $$@

endef

$(foreach file, $(HAL_FILES), $(eval $(call halSrcRules,$(patsubst %.c,%.o,$(file)),$(file))))

ifeq '$(pulp_chip_family)' 'vega'
CHIP_TARGETS += gen_linker_script
endif

ifeq '$(pulp_chip_family)' 'gap9'
CHIP_TARGETS += gen_linker_script
endif


build_rt: build $(CHIP_TARGETS)

clean_all:
	make fullclean $(MK_OPT)
	make PULP_RT_CONFIG=configs/pulpos_profile.mk fullclean $(MK_OPT)

build_all:
	make build_rt install $(MK_OPT)
	make PULP_RT_CONFIG=configs/pulpos_profile.mk build install $(MK_OPT)

gen_linker_script:
	./rules/process_linker_script --input=rules/$(pulp_chip_family)/link.ld.in --output=$(TARGET_INSTALL_DIR)/rules/$(pulp_chip)/link.ld --config=$(pulp_chip)

vega: $(CONFIG_BUILD_DIR)/link.ld