
# In case the architecture does not have any fabric controller, the FC code will run
# on pe0 of cluster0
fc_archi = $(fc/archi)
ifeq '$(fc/archi)' ''
fc_archi = $(pe/archi)
endif


PULP_LIB_FC_SRCS_rt     += kernel/init.c \
   kernel/dev.c kernel/irq.c kernel/debug.c \
   kernel/utils.c kernel/error.c kernel/bridge.c kernel/conf.c \
   kernel/log.c kernel/mem_slab.c

PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/thread.S

PULP_CFLAGS     += -D__RT_USE_BRIDGE=1

ifdef CONFIG_WARNING_ENABLED
PULP_CFLAGS     += -D__RT_USE_WARNING=1 -DPI_LOG_DEFAULT_DYNAMIC_LEVEL=2
else
PULP_CFLAGS     += -DPI_LOG_DEFAULT_DYNAMIC_LEVEL=0
endif

ifneq '$(cluster/version)' ''
PULP_LIB_FC_SRCS_rt     += kernel/task.c
PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/task.S
endif


ifeq '$(pulp_chip)' 'oprecompkw'
PULP_LIB_FC_ASM_SRCS_rt += kernel/oprecompkw/rt/crt0.S
else
PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/rt/crt0.S
endif

ifeq '$(CONFIG_SCHED_ENABLED)' '1'
PULP_LIB_FC_SRCS_rt     += kernel/thread.c kernel/events.c
endif

ifeq '$(CONFIG_CHECK_CLUSTER_START)' '1'
PULP_CFLAGS             += -DCONFIG_CHECK_CLUSTER_START=1
endif


ifeq '$(CONFIG_ALLOC_ENABLED)' '1'
PULP_LIB_FC_SRCS_rt     += kernel/alloc.c kernel/alloc_extern.c
endif



ifeq '$(CONFIG_TIME_ENABLED)' '1'
PULP_LIB_FC_SRCS_rt     += kernel/time.c kernel/time_irq.c
endif



ifneq '$(fc_archi)' 'or1k'
PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/sched.S kernel/$(fc_archi)/vectors.S
endif

ifneq '$(udma/version)' ''
PULP_LIB_FC_SRCS_rt     += kernel/periph-v$(udma/archi).c
PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/udma-v$(udma/archi).S kernel/$(fc_archi)/udma_spim-v$(udma/spim/version).S
endif

ifneq '$(udma/mram/version)' ''
PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/udma_mram-v$(udma/mram/version).S
endif

ifneq '$(soc/fll/version)' ''
ifeq '$(pulp_chip_family)' 'gap'
PULP_LIB_FC_SRCS_rt     += kernel/gap/freq.c kernel/gap/pm.c
else
ifeq '$(pulp_chip)' 'arnold'
PULP_LIB_FC_SRCS_rt     += kernel/fll-v$(fll/version).c
PULP_LIB_FC_SRCS_rt     += kernel/freq-one-per-domain.c
else
ifeq '$(pulp_chip)' 'pulp'
PULP_LIB_FC_SRCS_rt     += kernel/fll-v$(fll/version).c
PULP_LIB_FC_SRCS_rt     += kernel/freq-one-per-domain.c
else
ifeq '$(pulp_chip_family)' 'pulpissimo'
PULP_LIB_FC_SRCS_rt     += kernel/fll-v$(fll/version).c
PULP_LIB_FC_SRCS_rt     += kernel/freq-one-per-domain.c
else
ifeq '$(pulp_chip_family)' 'pulpissimo_v2'
PULP_LIB_FC_SRCS_rt     += kernel/fll-v$(fll/version).c
PULP_LIB_FC_SRCS_rt     += kernel/freq-one-per-domain.c
else
ifeq '$(pulp_chip_family)' 'vega'
PULP_LIB_FC_SRCS_rt     += kernel/fll-v$(fll/version).c
PULP_LIB_FC_SRCS_rt     += kernel/freq-one-per-domain.c
else
ifeq '$(pulp_chip_family)' 'gap9'
PULP_LIB_FC_SRCS_rt     += kernel/fll-v$(fll/version).c
PULP_LIB_FC_SRCS_rt     += kernel/freq-one-per-domain.c
else
PULP_LIB_FC_SRCS_rt     += kernel/fll-v$(fll/version).c
PULP_LIB_FC_SRCS_rt     += kernel/freq-v$(fll/version).c
endif
endif
endif
endif
endif
endif
endif
endif

ifneq '$(soc_eu/version)' ''
ifneq '$(fc_itc/version)' ''
PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/soc_event_itc-v$(soc_eu/version).S
else
PULP_LIB_FC_ASM_SRCS_rt += kernel/$(fc_archi)/soc_event_eu.S
endif
endif

PULP_LIB_CL_ASM_SRCS_rt += kernel/$(fc_archi)/pe-eu-v$(event_unit/version).S

ifeq '$(event_unit/version)' '1'
PULP_LIB_FC_SRCS_rt += kernel/riscv/pe-eu-v1-entry.c
endif

ifeq '$(pulp_chip_family)' 'wolfe'
PULP_LIB_FC_SRCS_rt += kernel/wolfe/maestro.c
endif

ifeq '$(pulp_chip_family)' 'devchip'
PULP_LIB_FC_SRCS_rt += kernel/wolfe/maestro.c
endif

ifeq '$(pulp_chip_family)' 'vega'
PULP_LIB_FC_SRCS_rt += kernel/vega/maestro.c kernel/vega/maestro_irq.c kernel/vega/pad.c
endif

ifeq '$(pulp_chip_family)' 'gap9'
PULP_LIB_FC_SRCS_rt += kernel/gap9/maestro.c kernel/gap9/maestro_irq.c kernel/gap9/pad.c
endif

ifeq '$(pulp_chip_family)' 'gap'
PULP_LIB_FC_SRCS_rt += kernel/gap/maestro.c kernel/gap/pmu_driver.c
endif


ifeq '$(pulp_chip_family)' 'vivosoc3'
PULP_LIB_FC_SRCS_rt     += kernel/vivosoc3/fll.c 
PULP_LIB_FC_SRCS_rt     += kernel/vivosoc3/freq.c
endif

ifeq '$(pulp_chip_family)' 'vivosoc3_1'
PULP_LIB_FC_SRCS_rt     += kernel/vivosoc3/fll.c 
PULP_LIB_FC_SRCS_rt     += kernel/vivosoc3/freq.c
endif

ifeq '$(pulp_chip_family)' 'vivosoc4'
PULP_LIB_FC_SRCS_rt     += kernel/vivosoc4/fll.c 
PULP_LIB_FC_SRCS_rt     += kernel/vivosoc4/freq.c
endif


PULP_LIB_FC_SRCS_rt += kernel/cluster.c kernel/pulpos_emu.c

ifneq '$(cluster/version)' ''
PULP_CFLAGS += -D__RT_CLUSTER_ASM
PULP_LIB_FC_SRCS_rt += kernel/cluster_call.c
endif

ifneq '$(perf_counters)' ''
PULP_LIB_FC_SRCS_rt += kernel/perf.c
endif

ifneq '$(cluster/version)' ''
ifneq '$(event_unit/version)' '1'
PULP_LIB_CL_SRCS_rt += kernel/sync_mc.c
endif
endif

ifeq '$(pulp_chip_family)' 'pulpissimo'
PULP_LIB_FC_SRCS_rt += kernel/pulpissimo/pulpissimo.c	
endif

ifeq '$(pulp_chip_family)' 'pulp'
PULP_LIB_FC_SRCS_rt += kernel/pulp/pulp.c	
endif

INSTALL_TARGETS += $(INSTALL_DIR)/lib/$(pulp_chip)/$(PULP_LIB_NAME_rt)/crt0.o


ifeq '$(pulp_chip)' 'oprecompkw'

$(INSTALL_DIR)/lib/$(pulp_chip)/$(PULP_LIB_NAME_rt)/crt0.o: $(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/kernel/oprecompkw/rt/crt0.o
	install -D $< $@

else

$(INSTALL_DIR)/lib/$(pulp_chip)/$(PULP_LIB_NAME_rt)/crt0.o: $(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_rt)/fc/kernel/$(fc_archi)/rt/crt0.o
	install -D $< $@

endif
