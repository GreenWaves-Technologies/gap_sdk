# The C program compiler.
CXX         = riscv32-unknown-elf-g++
CC          = riscv32-unknown-elf-gcc
AR          = riscv32-unknown-elf-ar
OBJDUMP     = riscv32-unknown-elf-objdump
NM          = riscv32-unknown-elf-nm
SIZE        = riscv32-unknown-elf-size

# Print compile command
TRC_MAKE=@
ifeq ($(VERBOSE), 1)
TRC_MAKE=
endif				# VERBOSE

platform     ?= board

chip=$(TARGET_CHIP_FAMILY)
TARGET_CHIP_VERSION=
ifeq ($(TARGET_CHIP), GAP8)
TARGET_CHIP_VERSION=1
else ifeq ($(TARGET_CHIP), GAP8_V2)
TARGET_CHIP_VERSION=2
else
TARGET_CHIP_VERSION=1
endif
chip_lowercase = $(shell echo $(chip) | tr A-Z a-z)

# Directories
FREERTOS_CONFIG_DIR = $(FREERTOS_PATH)/demos/gwt/$(chip_lowercase)/common/config_files
FREERTOS_SOURCE_DIR = $(FREERTOS_PATH)/freertos_kernel
PORT_DIR            = $(FREERTOS_SOURCE_DIR)/portable/GCC/RI5CY-$(chip)/
GWT_DIR             = $(FREERTOS_PATH)/vendors/gwt
GWT_TARGET          = $(GWT_DIR)/TARGET_GWT
GWT_PMSIS           = $(GWT_TARGET)/pmsis
GWT_LIBS            = $(GWT_TARGET)/libs
GWT_DEVICE          = $(GWT_TARGET)/TARGET_$(chip)/device
GWT_DRIVER          = $(GWT_TARGET)/TARGET_$(chip)/driver
GWT_PMSIS_BACKEND   = $(GWT_PMSIS)/backend
GWT_PMSIS_IMPLEM    = $(GWT_PMSIS)/implem
ifeq ($(GAP_SDK_HOME), )
GWT_PMSIS_API       = $(GWT_PMSIS)/api
else
GWT_PMSIS_API       = $(GAP_SDK_HOME)/rtos/pmsis/pmsis_api
endif				# GAP_SDK_HOME

ifeq ($(chip), GAP8)
APP_ARCH_CFLAGS     ?= -mchip=gap8 -mPE=8 -mFC=1
else
APP_ARCH_CFLAGS     ?= -mchip=gap9 -mPE=8 -mFC=1
endif				# chip
APP_ARCH_LDFLAGS    ?=

RISCV_FLAGS         ?= $(APP_ARCH_CFLAGS)
FREERTOS_FLAGS      += -D__riscv__ -D__$(chip)__ -D__RISCV_ARCH_GAP__=1 \
                       -DCHIP_VERSION=$(TARGET_CHIP_VERSION)

# Simulation related options
export PULP_CURRENT_CONFIG_ARGS += $(CONFIG_OPT)

# Main stack size in Bytes.
MAIN_STACK_SIZE     ?= 2048
MAIN_APP_STACK_SIZE  = $(MAIN_STACK_SIZE)

# Option to use cluster features
FEATURE_FLAGS   = -DFEATURE_CLUSTER=1

# Option to use preemptive mode
ifeq ($(NO_PREEMPTION), true)
FREERTOS_FLAGS  +=
else
FREERTOS_FLAGS  += -DPREEMPTION
endif


# Simulation platform
# Default is gapuino

ifdef PLPTEST_PLATFORM
platform=$(PLPTEST_PLATFORM)
ifneq ($(platform), gvsoc)
use_pulprun=1
endif				# platform
endif				# PLPTEST_PLATFORM

# GVSOC
ifeq ($(platform), gvsoc)
GVSOC_FILES_CLEAN   = all_state.txt core_state.txt rt_state.txt  \
                      efuse_preload.data plt_config.json stimuli \
                      tx_uart.log
FREERTOS_FLAGS  += -D__PLATFORM_GVSOC__ #-DPRINTF_RTL
io = rtl

# FPGA
else ifeq ($(platform), fpga)
FREERTOS_FLAGS  += -D__PLATFORM_FPGA__
io ?= host

# RTL
else ifeq ($(platform), rtl)
GUI =
FREERTOS_FLAGS  += -D__PLATFORM_RTL__
FREERTOS_FLAGS  += -DPRINTF_RTL
io = rtl

ifeq ($(GUI), 1)
override runner_args += --gui
endif				# GUI

endif				# platform

# Choose Simulator (use Xcelium unless specified otherwise)
ifeq ($(sim), vsim)
export GAPY_RTL_SIMULATOR = vsim
RTL_SIMU_DEBUG            = vsim -view $(BUILDDIR)/vsim.wlf
else
export GAPY_RTL_SIMULATOR = xcelium
RTL_SIMU_DEBUG            = simvision -waves $(BUILDDIR)/waves.shm
endif				# sim

export GAP_USE_OPENOCD=1
# Default is semihosting
io ?= host

# No printf
ifeq ($(io), disable)
IO                  = -D__DISABLE_PRINTF__
endif

# Printf using semihosting
ifeq ($(io), host)
FREERTOS_FLAGS     += -D__SEMIHOSTING__
IO                  = -DPRINTF_SEMIHOST
endif

# Printf using uart
ifeq ($(io), uart)
IO                  = -DPRINTF_UART -DPRINTF_SEMIHOST
MAIN_APP_STACK_SIZE    := $(shell expr $(MAIN_APP_STACK_SIZE) + 1024)
endif

# Printf using stdout
ifeq ($(io), rtl)
IO                  = -DPRINTF_RTL
endif

# Enabled for gvsoc
#FREERTOS_FLAGS     += -DPRINTF_RTL

# The pre-processor and compiler options.
# Users can override those variables from the command line.
FREERTOS_FLAGS     += -D__FREERTOS__ -DTOOLCHAIN_GCC_RISCV -DTOOLCHAIN_GCC

# App task stack size.
FREERTOS_FLAGS     += -DMAIN_APP_STACK_SIZE=$(MAIN_APP_STACK_SIZE)

COMMON              = -c -g -fmessage-length=0 -fno-exceptions -fno-builtin \
                      -ffunction-sections -fdata-sections -funsigned-char \
                      -fno-delete-null-pointer-checks -fomit-frame-pointer -Os \
                      $(DEVICE_FLAGS) $(FEATURE_FLAGS) $(RISCV_FLAGS) $(FREERTOS_FLAGS)

GCC_OPTIM_LEVEL     = -Os	# Optimize for size.

# Enable log/traces. Often another flag should be set in order to print traces.
DEBUG_FLAGS         = -DPI_LOG_DEFAULT_LEVEL=PI_LOG_TRACE

PRINTF_FLAGS        = -DPRINTF_ENABLE_LOCK #-DPRINTF_DISABLE_SUPPORT_EXPONENTIAL #\
                      -DPRINTF_DISABLE_SUPPORT_FLOAT
PRINTF_FLAGS       += $(IO)

WARNINGS            = -Wall -Wextra -Wno-unused-parameter -Wno-unused-function \
                      -Wno-unused-variable -Wno-unused-but-set-variable \
                      -Wno-missing-field-initializers -Wno-format -Wimplicit-fallthrough=0

ASMFLAGS            = -x assembler-with-cpp $(COMMON) $(WARNINGS) -DASSEMBLY_LANGUAGE

CFLAGS              = -std=gnu99 $(COMMON) $(WARNINGS) $(PRINTF_FLAGS) $(DEBUG_FLAGS)

# Objdump options(disassembly).
#OBJDUMP_OPT         = -D -l -f -g -z
OBJDUMP_OPT         = -d -h -S -t -w --show-raw-insn

# NM options.
NM_OPT              = -a -A -l -S --size-sort --special-syms

# Size options.
SIZE_OPT            = -B -x --common

# The linker options.
# The options used in linking as well as in any direct use of ld.
LIBS                = -lgcc
STRIP               = -Wl,--gc-sections,-Map=$@.map,-static #,-s
ifeq ($(LINK_SCRIPT),)
LINK_SCRIPT         = $(GWT_DEVICE)/ld/$(chip).ld
endif				# LINK_SCRIPT
LDFLAGS             = -nostartfiles -nostdlib -T$(LINK_SCRIPT) $(STRIP) $(LIBS)

# libc/gcc CRT files.
GCC_CRT             = $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtbegin.o \
                      $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crti.o \
                      $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtn.o \
                      $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtend.o

# Sources and Includes.
CRT0_SRC            = $(shell find $(GWT_DEVICE) -iname "*.S")
PORT_ASM_SRC        = $(shell find $(PORT_DIR) -iname "*.S")

RTOS_SRC            = $(FREERTOS_SOURCE_DIR)/list.c \
                      $(FREERTOS_SOURCE_DIR)/queue.c \
                      $(FREERTOS_SOURCE_DIR)/tasks.c \
                      $(FREERTOS_SOURCE_DIR)/timers.c \
                      $(FREERTOS_SOURCE_DIR)/event_groups.c \
                      $(FREERTOS_SOURCE_DIR)/stream_buffer.c

RTOS_SRC           += $(FREERTOS_CONFIG_DIR)/FreeRTOS_util.c

PORT_SRC            = $(shell find $(PORT_DIR) -iname "*.c")
DEVICE_SRC          = $(shell find $(GWT_DEVICE) -iname "*.c")
DRIVER_SRC          = $(shell find $(GWT_DRIVER) -iname "*.c")
LIBS_SRC            = $(shell find $(GWT_LIBS)/src -iname "*.c")
PRINTF_SRC          = $(GWT_LIBS)/printf/printf.c

INC_PATH            = $(FREERTOS_SOURCE_DIR)/include
INC_PATH           += $(FREERTOS_CONFIG_DIR) $(PORT_DIR)
INC_PATH           += $(GWT_TARGET) $(GWT_DEVICE) $(GWT_DRIVER)
INC_PATH           += $(GWT_LIBS)/include
INC_PATH           += $(GWT_LIBS)/printf


# PMSIS
PMSIS_IMPLEM_DIR    = $(GWT_PMSIS_IMPLEM)
-include $(GWT_PMSIS_IMPLEM)/pmsis_implem.mk
PMSIS_BACKEND_SRC   = $(shell find $(GWT_PMSIS_BACKEND) -iname "*.c")
PMSIS_SRC           = $(PMSIS_IMPLEM_SRCS)
PMSIS_SRC          += $(PMSIS_BSP_SRCS)
PMSIS_ASM_SRC       = $(PMSIS_IMPLEM_ASM_SRCS)

PMSIS_INC_PATH      = $(GWT_PMSIS) $(GWT_PMSIS_API)/include/
PMSIS_INC_PATH     += $(GWT_PMSIS_BACKEND)
PMSIS_INC_PATH     += $(PMSIS_IMPLEM_INC)
PMSIS_INC_PATH     += $(PMSIS_BSP_INC)


# GAP_LIB sources
ifeq '$(CONFIG_GAP_LIB)' '1'
GAP_LIB_PATH        = $(GAP_SDK_HOME)/libs/gap_lib
GAP_LIB_SRC        += $(GAP_LIB_PATH)/img_io/ImgIO.c
GAP_LIB_INC_PATH   += $(GAP_LIB_PATH)/include $(GAP_LIB_PATH)/include/gaplib
endif				# GAP_LIB


# OpenMP sources
ifeq '$(CONFIG_OPENMP)' '1'
OPENMP_SRC          = $(OPENMP_SRCS)
OPENMP_INC_PATH     = $(OPENMP_INC)
CFLAGS             += -fopenmp
endif				# OpenMP


INCLUDES            = $(foreach f, $(INC_PATH) $(PMSIS_INC_PATH), -I$f)
INCLUDES           += $(foreach f, $(GAP_LIB_INC_PATH) $(OPENMP_INC_PATH), -I$f)
INCLUDES           += $(FEAT_INCLUDES)

# App sources
APP_SRC            +=
# App includes
APP_INCLUDES       += $(foreach f, $(APP_INC_PATH), -I$f)
# App compiler options
APP_CFLAGS         +=
# App linker options
APP_LDFLAGS        +=

# Directory containing built objects
BUILDDIR            = $(shell pwd)/BUILD$(build_dir_ext)/$(TARGET_CHIP)/GCC_RISCV

# Objects
CRT0_OBJ            = $(patsubst %.S, $(BUILDDIR)/%.o, $(CRT0_SRC))
PMSIS_ASM_OBJ       = $(patsubst %.S, $(BUILDDIR)/%.o, $(PMSIS_ASM_SRC))
PORT_ASM_OBJ        = $(patsubst %.S, $(BUILDDIR)/%.o, $(PORT_ASM_SRC))

APP_OBJ             = $(patsubst %.c, $(BUILDDIR)/%.o, $(APP_SRC))
DEVICE_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(DEVICE_SRC))
DRIVER_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(DRIVER_SRC))
GAP_LIB_OBJ         = $(patsubst %.c, $(BUILDDIR)/%.o, $(GAP_LIB_SRC))
LIBS_OBJ            = $(patsubst %.c, $(BUILDDIR)/%.o, $(LIBS_SRC))
OPENMP_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(OPENMP_SRC))
PMSIS_OBJ           = $(patsubst %.c, $(BUILDDIR)/%.o, $(PMSIS_SRC))
PMSIS_BACKEND_OBJ   = $(patsubst %.c, $(BUILDDIR)/%.o, $(PMSIS_BACKEND_SRC))
PORT_OBJ            = $(patsubst %.c, $(BUILDDIR)/%.o, $(PORT_SRC))
PRINTF_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(PRINTF_SRC))
RTOS_OBJ            = $(patsubst %.c, $(BUILDDIR)/%.o, $(RTOS_SRC))

ASM_OBJS            = $(CRT0_OBJ) $(PMSIS_ASM_OBJ) $(PORT_ASM_OBJ)
C_OBJS              = $(API_OBJ) $(DEVICE_OBJ) $(DRIVER_OBJ) $(GAP_LIB_OBJ) \
                      $(LIBS_OBJ) $(OPENMP_OBJ) $(PMSIS_OBJ) \
                      $(PMSIS_BACKEND_OBJ) $(PORT_OBJ) $(PRINTF_OBJ) $(RTOS_OBJ)

# Objects to build.
BUILDING_OBJS       = $(APP_OBJ) $(ASM_OBJS) $(C_OBJS)
# In case there are duplicate sources and user wants to use his own sources.
# User can exclude some source from build, using APP_EXCLUDE_SRCS.
APP_EXCLUDE_OBJS    = $(patsubst %.c, $(BUILDDIR)/%.o, $(APP_EXCLUDE_SRCS))
# Final objects to build.
OBJS                = $(filter-out $(APP_EXCLUDE_OBJS), $(BUILDING_OBJS))
# Objects disassembly.
OBJS_DUMP           = $(patsubst %.o, %.dump, $(OBJS))
# Objects dependency.
OBJS_DEP            = $(patsubst %.o, %.d, $(OBJS))
# Binary file name.
APP                ?= test
BIN                 = $(BUILDDIR)/$(APP)

# Makefile targets :
# Build objects (*.o) amd associated dependecies (*.d) with disassembly (*.dump).
#------------------------------------------

-include $(OBJS_DEP)

all:: $(OBJS) $(BIN) image flash

$(BUILDDIR):
	mkdir -p $@

$(ASM_OBJS): $(BUILDDIR)/%.o: %.S
	@echo "    ASM  $(shell basename $<)"
	@mkdir -p $(dir $@)
	$(TRC_MAKE)$(CC) $(ASMFLAGS) $(INCLUDES) -MD -MF $(basename $@).d -o $@ $<

$(C_OBJS): $(BUILDDIR)/%.o: %.c
	@echo "    CC  $(shell basename $<)"
	@mkdir -p $(dir $@)
	$(TRC_MAKE)$(CC) $(CFLAGS) $(APP_CFLAGS) $(GCC_OPTIM_LEVEL) $(INCLUDES) -MD -MF $(basename $@).d -o $@ $<

$(APP_OBJ): $(BUILDDIR)/%.o: %.c
	@echo "    CC $(shell basename $<)"
	@mkdir -p $(dir $@)
	$(TRC_MAKE)$(CC) $(CFLAGS) $(APP_CFLAGS) $(INCLUDES) $(APP_INCLUDES) -MD -MF $(basename $@).d -o $@ $<

$(BIN): $(OBJS)
	$(TRC_MAKE)$(CC) $(APP_ARCH_LDFLAGS) -MMD -MP -o $@ $(GCC_CRT) $(OBJS) $(LDFLAGS) $(APP_LDFLAGS)

$(OBJS_DUMP): $(BUILDDIR)/%.dump: $(BUILDDIR)/%.o
	@$(OBJDUMP) $(OBJDUMP_OPT) $< > $@

$(BIN).s: $(BIN)
	@echo "    OBJDUMP  $(shell basename $<) > $@"
	@$(OBJDUMP) $(OBJDUMP_OPT) $< > $@

$(BIN).size: $(BIN)
	@echo "    DUMP SYMBOLS AND SIZE  $(shell basename $<) > $@"
	@$(SIZE) $(SIZE_OPT) $< > $@
	@echo -e "\n" >> $@
	@$(NM) $(NM_OPT) $< >> $@


override config_args += $(foreach file, $(READFS_FILES), --config-opt=flash/content/partitions/readfs/files=$(file))
override config_args += $(foreach file, $(HOSTFS_FILES), --config-opt=flash/content/partitions/hostfs/files=$(file))

ifdef LFS_ROOT_DIR
override config_args += --config-opt=flash/content/partitions/lfs/root_dir=$(LFS_ROOT_DIR)
endif

flash: $(BIN)
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --flash --binary=$(BIN) $(runner_args)

image: $(BIN)
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --image --binary=$(BIN) $(runner_args)

run: $(BIN)
	gapy --target=$(GAPY_TARGET) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) run --exec-prepare --exec --binary=$(BIN) $(runner_args)

disdump: $(OBJS_DUMP) $(BIN).s $(BIN).size

debug:
	$(RTL_SIMU_DEBUG)

clean:: clean_app
	@rm -rf $(OBJS) $(DUMP)
	@rm -rf *~ BUILD$(build_dir_ext) transcript *.wav __pycache__
	@rm -rf $(GVSOC_FILES_CLEAN) .simvision
	@rm -rf version.log

clean_app::
	@rm -rf $(APP_OBJ) $(BIN) $(OBJS_DUMP)

.PHONY: clean all run debug disdump clean_app
