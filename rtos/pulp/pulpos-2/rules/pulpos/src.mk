
define include_target

-include $(1)/rules/pulpos/src.mk

endef


$(foreach module, $(PULPOS_MODULES), $(eval $(call include_target,$(module))))



ifeq '$(CONFIG_CRT0)' '1'
PULP_ASM_SRCS += kernel/crt0.S
endif

ifeq '$(CONFIG_LIBC_MINIMAL)' '1'
PULP_SRCS += lib/libc/minimal/io.c lib/libc/minimal/fprintf.c lib/libc/minimal/prf.c lib/libc/minimal/sprintf.c lib/libc/minimal/semihost.c
endif

ifdef CONFIG_KERNEL
PULP_SRCS += kernel/init.c kernel/kernel.c kernel/device.c kernel/task.c kernel/alloc.c \
	kernel/alloc_pool.c kernel/irq.c kernel/soc_event.c kernel/log.c kernel/time.c

PULP_ASM_SRCS += kernel/irq_asm.S kernel/time_asm.S

endif


# HYPER

ifeq '$(CONFIG_HYPER)' '1'
ifneq '$(udma/version)' ''
ifeq '$(TARGET_CHIP_FAMILY)' 'GAP9'
HYPER_HAS_ASM = 1
HYPER_HAS_OCTOSPI = 1
endif
CONFIG_UDMA = 1
PULP_SRCS += drivers/hyperbus/hyperbus-v$(udma/hyper/version).c
ifeq '$(HYPER_HAS_ASM)' '1'
PULP_ASM_SRCS += drivers/hyperbus/hyperbus-v$(udma/hyper/version)_asm.S
endif
ifeq '$(TARGET_CHIP)' 'GAP9_V2'
PULP_SRCS += drivers/hyperbus/hyperbus-v$(udma/hyper/version)_irq.c
endif
ifeq '$(HYPER_HAS_OCTOSPI)' '1'
PULP_SRCS += drivers/octospi/octospi-v$(udma/hyper/version).c
PULP_ASM_SRCS += drivers/octospi/octospi-v$(udma/hyper/version)_asm.S
endif
endif
endif


# SPIM

ifeq '$(CONFIG_SPIM)' '1'
ifneq '$(udma/version)' ''
CONFIG_UDMA = 1
PULP_SRCS += drivers/spim/spim-v$(udma/spim/version).c
ifeq '$(CONFIG_USE_ASM_OPTIM)' '1'
PULP_ASM_SRCS += drivers/spim/spim-v$(udma/spim/version)_asm.S
endif
endif
endif


# UART

ifeq '$(CONFIG_UART)' '1'
ifneq '$(udma/uart/version)' ''
CONFIG_UDMA = 1
PULP_SRCS += drivers/uart/uart-v$(udma/uart/version).c
endif
endif


# I2S

ifeq '$(CONFIG_I2S)' '1'
ifneq '$(udma/i2s/version)' ''
CONFIG_UDMA = 1
PULP_SRCS += drivers/i2s/i2s-v$(udma/i2s/version).c
endif
endif


# I2C

ifeq '$(CONFIG_I2C)' '1'
ifneq '$(udma/i2c/version)' ''
CONFIG_UDMA = 1
PULP_SRCS += drivers/i2c/i2c-v$(udma/i2c/version).c
endif
endif



# UDMA
ifeq '$(CONFIG_UDMA)' '1'
ifneq '$(udma/version)' ''
PULP_CFLAGS   += -D__CONFIG_UDMA__
PULP_SRCS     += drivers/udma/udma-v$(udma/archi).c
endif
endif



# GPIO
ifeq '$(CONFIG_GPIO)' '1'
ifneq '$(gpio/version)' ''
PULP_SRCS     += drivers/gpio/gpio-v$(gpio/version).c
PULP_ASM_SRCS     += drivers/gpio/gpio-v$(gpio/version)_asm.S
endif
endif


# PADS
ifeq '$(CONFIG_PADS)' '1'
ifneq '$(padframe/version)' ''
PULP_SRCS += drivers/pads/pads-v$(padframe/version).c
endif
endif


# Cluster
ifeq '$(CONFIG_CLUSTER)' '1'
ifneq '$(cluster/version)' ''
PULP_SRCS += drivers/cluster/cluster.c
PULP_ASM_SRCS += drivers/cluster/pe-eu-v$(event_unit/version).S
ifneq '$(event_unit/version)' '3'
PULP_ASM_SRCS += drivers/cluster/pe-eu-v$(event_unit/version)_task.S
endif
endif
endif



# PULP_CL_ASM_SRCS += kernel/riscv/pe-eu-v$(event_unit/version).S
# PULP_ASM_SRCS += kernel/$(fc/archi)/sched.S kernel/$(fc/archi)/vectors.S

# PULP_SRCS += kernel/init.c kernel/irq.c kernel/utils.c kernel/alloc.c kernel/events.c kernel/bridge.c
# PULP_SRCS += kernel/time.c kernel/time_irq.c

# ifneq '$(soc_eu/version)' ''
# ifneq '$(fc_itc/version)' ''
# PULP_ASM_SRCS += kernel/$(fc/archi)/soc_event_itc-v$(soc_eu/version).S
# else
# PULP_ASM_SRCS += kernel/$(fc/archi)/soc_event_eu.S
# endif
# endif

# ifneq '$(cluster/version)' ''
# PULP_SRCS += kernel/cluster.c
# PULP_CFLAGS += -D__RT_CLUSTER_ASM
# PULP_SRCS += kernel/cluster_call.c
# endif

# ifneq '$(udma/uart/version)' ''
# PULP_SRCS += drivers_deprecated/uart/uart.c
# endif

# # HYPER
# ifneq '$(udma/hyper/version)' ''
# PULP_CFLAGS += -D__RT_HYPER_COPY_ASM=1
# PULP_SRCS += drivers/hyper/hyperram-v$(udma/hyper/version).c
# PULP_ASM_SRCS += drivers/hyper/hyperram-v$(udma/hyper/version)_asm.S
# endif


# # RTC
# PULP_FC_CFLAGS += -DRT_CONFIG_RTC_ENABLED
# ifneq '$(rtc/version)' ''
# ifneq '$(rtc/version)' '1'
# PULP_SRCS += drivers_deprecated/rtc/rtc_v$(rtc/version).c
# PULP_ASM_SRCS += drivers_deprecated/rtc/rtc_v$(rtc/version)_asm.S
# else
# PULP_SRCS += drivers_deprecated/dolphin/rtc.c
# endif
# endif
