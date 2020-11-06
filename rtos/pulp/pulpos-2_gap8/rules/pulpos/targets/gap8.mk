PULP_LDFLAGS      += 
PULP_CFLAGS       +=  -D__riscv__
PULP_ARCH_CFLAGS ?=  -march=rv32imcxgap8 -mPE=8 -mFC=1
PULP_ARCH_LDFLAGS ?=  -march=rv32imcxgap8 -mPE=8 -mFC=1
PULP_ARCH_OBJDFLAGS ?= -Mmarch=rv32imcxgap8
PULP_CFLAGS    += -Werror -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections
PULP_CFLAGS    += -include pos/chips/gap8/config.h -I$(PULPOS_GAP8_HOME)/include/pos/chips/gap8 -I$(PULPOS_HOME)/include -I$(GAP_PULPOS_ARCHI)/include
PULP_OMP_CFLAGS    += -fopenmp -mnativeomp
PULP_LDFLAGS += -nostartfiles -nostdlib -Wl,--gc-sections -L$(PULPOS_GAP8_HOME)/kernel -Tchips/gap8/link.ld -lgcc

PULP_CC = riscv32-unknown-elf-gcc 
PULP_AR ?= riscv32-unknown-elf-ar
PULP_LD ?= riscv32-unknown-elf-gcc
PULP_OBJDUMP ?= riscv32-unknown-elf-objdump

fc/archi=riscv
pe/archi=riscv
pulp_chip=gap
pulp_chip_family=gap
cluster/version=5
udma/hyper/version=1
udma/cpi/version=1
udma/i2c/version=2
soc/fll/version=1
udma/i2s/version=1
udma/uart/version=1
event_unit/version=3
perf_counters=True
fll/version=1
padframe/version=1
udma/spim/version=2
gpio/version=2
udma/archi=2
soc_eu/version=1
rtc/version=1
udma/version=2

PULP_SRCS     += kernel/freq-domains.c
PULP_SRCS     += kernel/chips/gap8/soc.c
PULP_SRCS     += kernel/chips/gap8/pmu_driver.c

include $(PULPOS_HOME)/rules/pulpos/configs/default.mk
include $(PULPOS_HOME)/rules/pulpos/default_rules.mk