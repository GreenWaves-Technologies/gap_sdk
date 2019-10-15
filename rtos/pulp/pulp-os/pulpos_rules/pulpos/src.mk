ifeq '$(CONFIG_CRT0)' '1'
PULP_ASM_SRCS += kernel/riscv/rt/crt0.S
endif

PULP_CL_ASM_SRCS += kernel/riscv/pe-eu-v$(event_unit/version).S
PULP_ASM_SRCS += kernel/$(fc/archi)/sched.S kernel/$(fc/archi)/vectors.S

PULP_SRCS += kernel/init.c kernel/irq.c kernel/utils.c kernel/alloc.c kernel/events.c kernel/bridge.c
PULP_SRCS += kernel/time.c kernel/time_irq.c

ifneq '$(soc_eu/version)' ''
ifneq '$(fc_itc/version)' ''
PULP_ASM_SRCS += kernel/$(fc/archi)/soc_event_itc-v$(soc_eu/version).S
else
PULP_ASM_SRCS += kernel/$(fc/archi)/soc_event_eu.S
endif
endif

ifeq '$(pulp_chip_family)' 'gap'
PULP_SRCS += kernel/gap/maestro.c kernel/gap/pmu_driver.c
endif

ifeq '$(pulp_chip_family)' 'gap'
PULP_SRCS += kernel/gap/freq.c kernel/gap/pm.c
endif

ifneq '$(cluster/version)' ''
PULP_SRCS += kernel/cluster.c
PULP_CFLAGS += -D__RT_CLUSTER_ASM
PULP_SRCS += kernel/cluster_call.c
endif

ifneq '$(udma/uart/version)' ''
PULP_SRCS += drivers_deprecated/uart/uart.c
endif

ifneq '$(udma/version)' ''
PULP_SRCS += kernel/periph-v$(udma/archi).c
PULP_ASM_SRCS += kernel/$(fc/archi)/udma-v$(udma/archi).S kernel/$(fc/archi)/udma_spim-v$(udma/spim/version).S
endif

PULP_SRCS += libs/io/io.c libs/io/fprintf.c libs/io/prf.c libs/io/sprintf.c


# HYPER
ifneq '$(udma/hyper/version)' ''
PULP_CFLAGS += -D__RT_HYPER_COPY_ASM=1
PULP_SRCS += drivers/hyper/hyperram-v$(udma/hyper/version).c
PULP_ASM_SRCS += drivers/hyper/hyperram-v$(udma/hyper/version)_asm.S
endif


# RTC
PULP_FC_CFLAGS += -DRT_CONFIG_RTC_ENABLED
ifneq '$(rtc/version)' ''
ifneq '$(rtc/version)' '1'
PULP_SRCS += drivers_deprecated/rtc/rtc_v$(rtc/version).c
PULP_ASM_SRCS += drivers_deprecated/rtc/rtc_v$(rtc/version)_asm.S
else
PULP_SRCS += drivers_deprecated/dolphin/rtc.c
endif
endif

# PWM
ifeq '$(pulp_chip_family)' 'gap'
PULP_CFLAGS += -DRT_CONFIG_PWM_ENABLED
PULP_SRCS += drivers/pwm/pwm.c
PULP_ASM_SRCS += drivers/pwm/pwm_asm.S
endif
