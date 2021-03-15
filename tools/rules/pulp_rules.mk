# The C program compiler.
CXX           = riscv32-unknown-elf-g++
CC            = riscv32-unknown-elf-gcc
AR            = riscv32-unknown-elf-ar
OBJDUMP       = riscv32-unknown-elf-objdump
NM          = riscv32-unknown-elf-nm
SIZE        = riscv32-unknown-elf-size

platform     ?= board

# The linker options.
ifeq ($(CUSTOM_BSP),)
LIBS += -L$(TARGET_INSTALL_DIR)/lib/gap/$(BOARD_NAME) -lpibsp
endif

CONFIG_PULPRT_LIB ?= rt

ifneq (,$(filter $(TARGET_CHIP), GAP8 GAP8_V2 GAP8_V3))
LIBS          += -L$(TARGET_INSTALL_DIR)/lib/gap \
				-l$(CONFIG_PULPRT_LIB) -lrtio -l$(CONFIG_PULPRT_LIB) -lgcc
else
LIBS          += -L$(TARGET_INSTALL_DIR)/lib/vega \
				-l$(CONFIG_PULPRT_LIB) -lrtio -l$(CONFIG_PULPRT_LIB) -lgcc
endif


ifdef gui
override runner_args += --gui
endif

MAIN_STACK_SIZE ?= 2048
PULP_CFLAGS  += -DRT_FC_STACK_SIZE=$(MAIN_STACK_SIZE)

LIBSFLAGS     += -nostartfiles -nostdlib

ifndef CONFIG_NO_LDSCRIPT

# The options used in linking as well as in any direct use of ld.
ifeq ($(TARGET_CHIP), GAP8)
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap8.ld
else ifeq ($(TARGET_CHIP), GAP8_V2)
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap8_rev1.ld
else ifeq ($(TARGET_CHIP), GAP8_V3)
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap8_rev2.ld
else
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap9.ld
endif


ifeq ($(platform), gvsoc)
LDFLAGS       += -T$(INSTALL_DIR)/ld/gvsoc.conf.ld
PULP_CFLAGS += -D__PLATFORM_GVSOC__
else ifeq ($(platform), fpga)
LDFLAGS       += -T$(INSTALL_DIR)/ld/fpga.conf.ld
PULP_CFLAGS += -D__PLATFORM_FPGA__
else ifeq ($(platform), rtl)
LDFLAGS       += -T$(INSTALL_DIR)/ld/rtl.conf.ld
PULP_CFLAGS += -D__PLATFORM_RTL__
else
LDFLAGS       += -T$(INSTALL_DIR)/ld/gapuino.conf.ld
PULP_CFLAGS += -D__PLATFORM_BOARD__
endif

endif

LDFLAGS += $(PULP_LDFLAGS)

TARGET_CHIP_VERSION=
ifeq ($(TARGET_CHIP), GAP8)
TARGET_CHIP_VERSION=1
else ifeq ($(TARGET_CHIP), GAP8_V2)
TARGET_CHIP_VERSION=2
else ifeq ($(TARGET_CHIP), GAP8_V3)
TARGET_CHIP_VERSION=3
else
TARGET_CHIP_VERSION=1
endif

ifeq ($(TARGET_CHIP_FAMILY), GAP8)
APP_ARCH_CFLAGS ?= -mchip=gap8 -mPE=8 -mFC=1
else
APP_ARCH_CFLAGS ?= -mchip=gap9 -mPE=8 -mFC=1
endif
APP_ARCH_LDFLAGS ?= -march=rv32imcxgap8 -mPE=8 -mFC=1

PULP_ARCH_CFLAGS ?= $(APP_ARCH_CFLAGS)
PULP_ARCH_LDFLAGS ?= $(APP_ARCH_LDFLAGS)

ifdef PULP_FC_ARCH_CFLAGS
PULP_ARCH_CFLAGS = $(PULP_FC_ARCH_CFLAGS)
endif

ifdef PULP_CL_ARCH_CFLAGS
PULP_ARCH_CFLAGS = $(PULP_CL_ARCH_CFLAGS)
endif

ifneq (,$(filter $(TARGET_CHIP), GAP8 GAP8_V2 GAP8_V3))
RISCV_FLAGS   ?= $(PULP_ARCH_CFLAGS) -D__riscv__ -D__GAP__ -D__$(TARGET_CHIP_FAMILY)__ -DCHIP_VERSION=$(TARGET_CHIP_VERSION)
GAP_FLAGS	 += -D__pulp__ -DCONFIG_GAP -D__PULP_OS__
else
RISCV_FLAGS   ?= $(PULP_ARCH_CFLAGS) -D__riscv__
GAP_FLAGS	 += -D__pulp__ -DCONFIG_VEGA -D__PULP_OS__
endif

RISCV_FLAGS += -mnativeomp


WRAP_FLAGS    = -Wl,--gc-sections

# Choose Simulator
SIMULATOR      = vsim

ifeq ($(sim), xcelium)
SIMULATOR      = xcelium
endif

# The pre-processor and compiler options.
# Users can override those variables from the command line.
COMMON        = -Os -g -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS)

ASMFLAGS      = $(COMMON) -DLANGUAGE_ASSEMBLY -MMD -MP -c

BOOTFLAGS	  = -Os -g -DUSE_AES -fno-jump-tables -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

CFLAGS        = $(COMMON) -MMD -MP -c

ifeq '$(platform)' 'board'
ifeq '$(TARGET_CHIP)' 'GAP9'
io ?= bridge
else
io ?= host
endif
endif

ifeq '$(io)' 'host'
PULP_CFLAGS += -D__RT_IODEV__=2
endif
ifeq '$(io)' 'uart'
PULP_CFLAGS += -D__RT_IODEV__=1
endif

TCFLAGS       = -fno-jump-tables -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

ifeq '$(CONFIG_OPENMP)' '1'
PULP_APP_SRCS += $(OPENMP_DIR)/lib/omp.c $(OPENMP_DIR)/lib/omp_gcc.c
PULP_CFLAGS += -fopenmp
endif

# Objdump options
OBJDUMP_OPT         = -d -h -S -t -w --show-raw-insn

# NM options.
NM_OPT              = -a -A -l -S --size-sort --special-syms

# Size options.
SIZE_OPT            = -B -x --common

# Final binary
#------------------------------------------
BUILDDIR      = $(shell pwd)/BUILD$(build_dir_ext)/$(TARGET_CHIP)/GCC_RISCV

APP          ?= test
BIN           = $(BUILDDIR)/$(APP)

T_OBJECTS_C   = $(patsubst %.c, $(BUILDDIR)/%.o, $(PULP_APP_FC_SRCS) $(PULP_APP_SRCS))

OBJECTS       = $(T_OBJECTS_C)

OBJECTS   += $(BUILDDIR)/pulp-os/conf.o

ifneq (,$(filter $(TARGET_CHIP), GAP8 GAP8_V2 GAP8_V3))
ifeq '$(TARGET_CHIP)' 'GAP8'
INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/rt/chips/gap/config.h
endif
ifeq '$(TARGET_CHIP)' 'GAP8_V2'
INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/rt/chips/gap_rev1/config.h
endif
ifeq '$(TARGET_CHIP)' 'GAP8_V3'
INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/rt/chips/gap8_revc/config.h
endif
else
INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/rt/chips/$(TARGET_NAME)/config.h
endif


INC           = $(TARGET_INSTALL_DIR)/include \
				$(TARGET_INSTALL_DIR)/include/io \
				$(INSTALL_DIR)/include
INC          +=	$(PULP_APP_INC_PATH)

INC_PATH      = $(foreach d, $(INC), -I$d)  $(INC_DEFINE)


build: $(OBJECTS) $(BIN) disdump | $(BUILDDIR)

all:: build image flash_fs

$(BUILDDIR):
	mkdir -p $@

# Rules for creating dependency files (.d).
#------------------------------------------
$(T_OBJECTS_C) : $(BUILDDIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(BUILDDIR)/pulp-os/conf.o: $(GAP_SDK_HOME)/rtos/pulp/pulp-os/kernel/conf.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(BIN): $(OBJECTS)
	$(CC) $(PULP_ARCH_LDFLAGS) -MMD -MP $(WRAP_FLAGS) $(PULP_LDFLAGS) $(INC_PATH) -o $(BIN) $(OBJECTS) $(LIBS) $(LDFLAGS) $(LIBSFLAGS) $(INC_DEFINE)

ifdef PLPTEST_PLATFORM
platform=$(PLPTEST_PLATFORM)
ifneq ($(platform), gvsoc)
use_pulprun=1
endif
endif

override config_args += $(foreach file, $(READFS_FILES), --config-opt=flash/content/partitions/readfs/files=$(file))
override config_args += $(foreach file, $(HOSTFS_FILES), --config-opt=flash/content/partitions/hostfs/files=$(file))

ifdef LFS_ROOT_DIR
override config_args += --config-opt=flash/content/partitions/lfs/root_dir=$(LFS_ROOT_DIR)
endif

flash:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --flash --force --binary=$(BIN) $(runner_args)

flash_fs:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --flash --binary=$(BIN) $(runner_args)

image:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --image --binary=$(BIN) $(runner_args)

run.prepare:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --exec-prepare --binary=$(BIN) $(runner_args)

run.exec:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --exec --binary=$(BIN) $(runner_args)

run:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --exec-prepare --exec --binary=$(BIN) $(runner_args)


profiler:
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) --config-opt="gvsoc/events/gen_gtkw=false" run --image --flash --exec-prepare --binary=$(BIN) --event=.*@all.bin --event-format=raw $(runner_args)
	cd $(BUILDDIR) && if [ -e all.bin ]; then rm all.bin; fi; mkfifo all.bin
	cd $(BUILDDIR) && export PULP_CONFIG_FILE=$(BUILDDIR)/gvsoc_config.json && profiler $(BUILDDIR) $(BIN) gvsoc_config.json

#$(INSTALL_DIR)/runner/run_gapuino.sh $(BUILDDIR) $(BIN) $(RAW_IMAGE_PLPBRIDGE_FLAGS)  $(PLPBRIDGE_FLAGS) $(PLPBRIDGE_EXTRA_FLAGS)

$(BIN).s: $(BIN)
	$(OBJDUMP) $(OBJDUMP_OPT) $< > $@
	$(SIZE) $(SIZE_OPT) $< > $(BIN).size
	$(NM) $(NM_OPT) $< >> $(BIN).size

disdump: $(BIN).s

clean::
	@rm -rf $(OBJECTS) $(PROGRAM)
	@rm -rf ./BUILD$(build_dir_ext) transcript *.wav __pycache__

.PHONY: all run debug disdump gdbserver
