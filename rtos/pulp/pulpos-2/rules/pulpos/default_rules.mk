APP ?= test

ifndef TARGET_BUILD_DIR
TARGET_BUILD_DIR = $(CURDIR)/build
ifdef build_dir_ext
TARGET_BUILD_DIR := $(TARGET_BUILD_DIR)$(build_dir_ext)
endif
endif
ifndef VERBOSE
V = @
endif

ifdef VERBOSE
override gapy_args += --config-opt=**/runner/verbose=true
endif

CONFIG_BUILD_DIR=$(TARGET_BUILD_DIR)

APP_BUILD_DIR = $(TARGET_BUILD_DIR)/$(APP)


ifdef PULP_RISCV_GCC_TOOLCHAIN_CI
PULP_CC := $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/$(PULP_CC)
PULP_LD := $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/$(PULP_LD)
else
ifdef PULP_RUNTIME_GCC_TOOLCHAIN
PULP_CC := $(PULP_RUNTIME_GCC_TOOLCHAIN)/bin/$(PULP_CC)
PULP_LD := $(PULP_RUNTIME_GCC_TOOLCHAIN)/bin/$(PULP_LD)
else
ifdef PULP_RISCV_GCC_TOOLCHAIN
PULP_CC := $(PULP_RISCV_GCC_TOOLCHAIN)/bin/$(PULP_CC)
PULP_LD := $(PULP_RISCV_GCC_TOOLCHAIN)/bin/$(PULP_LD)
endif
endif
endif

VPATH = $(PULPOS_HOME) $(PULPOS_MODULES)

include $(PULPOS_HOME)/rules/pulpos/src.mk

PULP_CFLAGS += -fno-jump-tables -fno-tree-loop-distribute-patterns

ifeq '$(CONFIG_LIBC_MINIMAL)' '1'
PULP_APP_CFLAGS += -I$(PULPOS_HOME)/lib/libc/minimal/include
endif
PULP_APP_CFLAGS += -I$(PULPOS_HOME)/include -I$(PULPOS_HOME)/kernel -I$(PULPOS_ARCHI)/include -I$(PULPOS_HAL)/include -I$(PMSIS_API)/include

PULP_APP_CFLAGS += $(foreach inc,$(PULPOS_MODULES),-I$(inc)/include)

ifdef PULPOS_PLATFORM
platform=$(PULPOS_PLATFORM)
endif


ifndef platform
platform=gvsoc
endif

override disopt ?= -d

# Objdump options
OBJDUMP_OPT         = -d -h -S -t -w --show-raw-insn

# NM options.
NM_OPT              = -a -A -l -S --size-sort --special-syms

# Size options.
SIZE_OPT            = -B -x --common

PULP_CFLAGS += -D__PULPOS2__

ifeq '$(platform)' 'gvsoc'
PULP_CFLAGS += -D__PLATFORM__=ARCHI_PLATFORM_GVSOC -D__PLATFORM_GVSOC__
endif
ifeq '$(platform)' 'board'
PULP_CFLAGS += -D__PLATFORM__=ARCHI_PLATFORM_BOARD -D__PLATFORM_BOARD__
endif
ifeq '$(platform)' 'rtl'
PULP_CFLAGS += -D__PLATFORM__=ARCHI_PLATFORM_RTL -D__PLATFORM_RTL__
endif
ifeq '$(platform)' 'fpga'
PULP_CFLAGS += -D__PLATFORM__=ARCHI_PLATFORM_FPGA -D__PLATFORM_FPGA__
endif

ifdef io
ifeq '$(io)' 'host'
CONFIG_IO_HOST = 1
endif
ifeq '$(io)' 'uart'
CONFIG_IO_UART = 1
endif
endif


ifdef CONFIG_NB_CLUSTER_PE
PULP_CFLAGS += -DARCHI_CLUSTER_NB_PE=$(CONFIG_NB_CLUSTER_PE)
override config_args += --config-opt=cluster/nb_pe=$(CONFIG_NB_CLUSTER_PE)
endif

ifdef CONFIG_IO_HOST
PULP_CFLAGS += -DPOS_CONFIG_IO_HOST=$(CONFIG_IO_HOST)
endif

ifdef CONFIG_IO_UART
PULP_CFLAGS += -DPOS_CONFIG_IO_UART=$(CONFIG_IO_UART)
endif

ifdef CONFIG_IO_UART_BAUDRATE
PULP_CFLAGS += -DPOS_CONFIG_IO_UART_BAUDRATE=$(CONFIG_IO_UART_BAUDRATE)
endif

ifdef CONFIG_IO_UART_ITF
PULP_CFLAGS += -DPOS_CONFIG_IO_UART_ITF=$(CONFIG_IO_UART_ITF)
endif

ifdef CONFIG_RISCV_GENERIC
PULP_CFLAGS += -D__RISCV_GENERIC__=1
endif

ifdef CONFIG_USE_ASM_OPTIM
PULP_CFLAGS  += -D__USE_ASM_OPTIM__=1
endif

ifdef CONFIG_TRACE_LEVEL
CONFIG_TRACE_LEVEL_INT = 0
ifeq '$(CONFIG_TRACE_LEVEL)' 'FATAL'
CONFIG_TRACE_LEVEL_INT = 1
endif
ifeq '$(CONFIG_TRACE_LEVEL)' 'ERROR'
CONFIG_TRACE_LEVEL_INT = 2
PI_LOG_LOCAL_LEVEL = 1
endif
ifeq '$(CONFIG_TRACE_LEVEL)' 'WARNING'
CONFIG_TRACE_LEVEL_INT = 3
PI_LOG_LOCAL_LEVEL = 2
endif
ifeq '$(CONFIG_TRACE_LEVEL)' 'INFO'
CONFIG_TRACE_LEVEL_INT = 4
PI_LOG_LOCAL_LEVEL = 3
endif
ifeq '$(CONFIG_TRACE_LEVEL)' 'DEBUG'
CONFIG_TRACE_LEVEL_INT = 5
PI_LOG_LOCAL_LEVEL = 4
endif
ifeq '$(CONFIG_TRACE_LEVEL)' 'TRACE'
CONFIG_TRACE_LEVEL_INT = 6
PI_LOG_LOCAL_LEVEL = 5
endif
PULP_CFLAGS += -D__TRACE_LEVEL__=$(CONFIG_TRACE_LEVEL_INT) -DPI_LOG_LOCAL_LEVEL=$(PI_LOG_LOCAL_LEVEL)
endif

ifdef CONFIG_TRACE_ALL
PULP_CFLAGS += -D__TRACE_ALL__=1 -DCONFIG_TRACE_ALL=1
endif

ifdef CONFIG_TRACE_SPIM
PULP_CFLAGS += -D__TRACE_SPIM__=1
endif

ifdef CONFIG_TRACE_FREQ
PULP_CFLAGS += -D__TRACE_FREQ__=1
endif

ifdef CONFIG_TRACE_ALLOC
PULP_CFLAGS += -D__TRACE_ALLOC__=1
endif

ifdef CONFIG_TRACE_INIT
PULP_CFLAGS += -D__TRACE_INIT__=1
endif

ifdef CONFIG_TRACE_UART
PULP_CFLAGS += -D__TRACE_UART__=1
endif

PULP_CFLAGS += $(foreach trace,$(CONFIG_TRACE),-DCONFIG_TRACE_$(trace)=1)


#
# RUNNER
#

ifdef gui
override runner_args += --gui
endif

READFS_FLASH ?= flash

override config_args += $(foreach file, $(READFS_FILES), --config-opt=$(READFS_FLASH)/content/partitions/readfs/files=$(file))
override config_args += $(foreach file, $(HOSTFS_FILES), --config-opt=flash/content/partitions/hostfs/files=$(file))


#
# PULP_APPS
#

define declare_app

$(eval PULP_APP_SRCS_$(1) += $(PULP_APP_SRCS) $(PULP_APP_FC_SRCS) $(PULP_SRCS) $(PULP_APP_CL_SRCS) $(PULP_CL_SRCS))
$(eval PULP_APP_ASM_SRCS_$(1) += $(PULP_APP_ASM_SRCS) $(PULP_ASM_SRCS) $(PULP_APP_CL_ASM_SRCS) $(PULP_CL_ASM_SRCS))
$(eval PULP_APP_OBJS_$(1) += $(patsubst %.c,$(TARGET_BUILD_DIR)/$(1)/%.o,$(PULP_APP_SRCS_$(1))))
$(eval PULP_APP_OBJS_$(1) += $(patsubst %.S,$(TARGET_BUILD_DIR)/$(1)/%.o,$(PULP_APP_ASM_SRCS_$(1))))

$(eval PULP_APP_CFLAGS_$(1) += $(PULP_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_APP_CFLAGS))
$(eval PULP_APP_LDFLAGS_$(1) += $(PULP_ARCH_LDFLAGS) $(PULP_LDFLAGS) $(PULP_APP_LDFLAGS))

-include $(PULP_APP_OBJS_$(1):.o=.d)

$(eval APP_BUILD_DIR_$(1) += $(TARGET_BUILD_DIR)/$(1))

$(TARGET_BUILD_DIR)/$(1)/%.o: $(TARGET_BUILD_DIR)/$(1)/%.c
	@echo "CC  $$<"
	$(V)mkdir -p `dirname $$@`
	$(V)$(PULP_CC) -c $$< -o $$@ -MMD -MP $(PULP_APP_CFLAGS_$(1))

$(TARGET_BUILD_DIR)/$(1)/%.o: %.c
	@echo "CC  $$<"
	$(V)mkdir -p `dirname $$@`
	$(V)$(PULP_CC) -c $$< -o $$@ -MMD -MP $(PULP_APP_CFLAGS_$(1))

$(TARGET_BUILD_DIR)/$(1)/%.o: %.cpp
	@echo "CXX $$<"
	$(V)mkdir -p `dirname $$@`
	$(V)$(PULP_CC) -c $< -o $@ -MMD -MP $(PULP_APP_CFLAGS_$(1))

$(TARGET_BUILD_DIR)/$(1)/%.o: %.S
	@echo "CC  $$<"
	$(V)mkdir -p `dirname $$@`
	$(V)$(PULP_CC) -c $$< -o $$@ -MMD -MP -DLANGUAGE_ASSEMBLY $(PULP_APP_CFLAGS_$(1))

$(TARGET_BUILD_DIR)/$(1)/$(1): $(PULP_APP_OBJS_$(1))
	@echo "LD  $$@"
	$(V)mkdir -p `dirname $$@`
	$(V)$(PULP_LD) -o $$@ $$^ -MMD -MP $(PULP_APP_LDFLAGS_$(1))

$(TARGET_INSTALL_DIR)/bin/$(1): $(TARGET_BUILD_DIR)/$(1)/$(1)
	@echo "CP  $$@"
	$(V)mkdir -p `dirname $$@`
	$(V)cp $$< $$@

TARGETS += $(TARGET_BUILD_DIR)/$(1)/$(1)
INSTALL_TARGETS += $(TARGET_INSTALL_DIR)/bin/$(1)
BIN = $(TARGET_BUILD_DIR)/$(1)/$(1)

endef




ifdef PULP_APP
PULP_APPS += $(PULP_APP)
endif

$(foreach app, $(PULP_APPS), $(eval $(call declare_app,$(app))))

conf:

$(BIN).s: $(BIN)
	$(PULP_OBJDUMP) $(OBJDUMP_OPT) $< > $@
	$(PULP_SIZE) $(SIZE_OPT) $< > $(BIN).size
	$(PULP_NM) $(NM_OPT) $< >> $(BIN).size

disdump: $(BIN).s

build: $(TARGETS)

image:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(TARGET_BUILD_DIR) $(config_args) $(gapy_args) run --image --binary=$(TARGETS) $(runner_args)

flash:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(TARGET_BUILD_DIR) $(config_args) $(gapy_args) run --flash --binary=$(TARGETS) $(runner_args)

all:: build disdump image flash

clean::
	@echo "RM  $(TARGET_BUILD_DIR)"
	$(V)rm -rf $(TARGET_BUILD_DIR)

run.prepare:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(TARGET_BUILD_DIR) $(config_args) $(gapy_args) run --exec-prepare --binary=$(TARGETS) $(runner_args)

run.exec:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(TARGET_BUILD_DIR) $(config_args) $(gapy_args) run --exec --binary=$(TARGETS) $(runner_args)

run:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(TARGET_BUILD_DIR) $(config_args) $(gapy_args) run --exec-prepare --exec --binary=$(TARGETS) $(runner_args)

traces:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(TARGET_BUILD_DIR) $(config_args) $(gapy_args) run --exec --binary=$(TARGETS) --no-run --extend-traces $(runner_args)

dis:
	$(PULP_OBJDUMP) $(PULP_ARCH_OBJDFLAGS) $(disopt) $(TARGETS)

size:
	$(PULPOS_HOME)/bin/pos-size --binary=$(TARGETS) --depth=10

profiler:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) --config-opt="gvsoc/events/gen_gtkw=false" run --image --flash --exec-prepare --binary=$(BIN) --event=.*@all.bin --event-format=raw $(runner_args)
	cd $(BUILDDIR) && if [ -e all.bin ]; then rm all.bin; fi; mkfifo all.bin
	cd $(BUILDDIR) && export PULP_CONFIG_FILE=$(BUILDDIR)/gvsoc_config.json && profiler $(BUILDDIR) $(BIN) gvsoc_config.json

help:
	@echo "Makefile options:"
	@echo "  CONFIG_TRACE_LEVEL=<level>    Activate traces for the specified level (0=none, 1=fatal, 2=error, 3=warning, 4=info, 5=debug, 6=trace)."
	@echo "  CONFIG_TRACE_ALL=1            Activate all traces. Other traces can be individually activated with CONFIG_TRACE_<NAME>."

.PHONY: image flash exec run dis size help clean all conf