# The C program compiler.
CXX         = riscv32-unknown-elf-g++
CC          = riscv32-unknown-elf-gcc
AR          = riscv32-unknown-elf-ar
OBJDUMP     = riscv32-unknown-elf-objdump

platform     ?= gapuino

chip=$(TARGET_CHIP_FAMILY)

# Directories
FREERTOS_CONFIG_DIR = $(FREERTOS_PATH)/demos/gwt/gap8/common/config_files
FREERTOS_SOURCE_DIR = $(FREERTOS_PATH)/freertos_kernel
PORT_DIR            = $(FREERTOS_SOURCE_DIR)/portable/GCC/RI5CY-GAP8/
GWT_DIR             = $(FREERTOS_PATH)/vendors/gwt
GWT_TARGET          = $(GWT_DIR)/TARGET_GWT
GWT_LIBS            = $(GWT_TARGET)/libs
GWT_DEVICE          = $(GWT_TARGET)/TARGET_$(chip)/device
GWT_DRIVER          = $(GWT_TARGET)/TARGET_$(chip)/driver
GWT_PMSIS           = $(GAP_SDK_HOME)/rtos/pmsis/pmsis_driver
GWT_PMSIS_API       = $(GAP_SDK_HOME)/rtos/pmsis/pmsis_api
PMSIS_BACKEND       = $(GWT_TARGET)/pmsis_backend

# The linker options.
LIBS            += -lgcc
LIBSFLAGS       += -nostartfiles -nostdlib

# The options used in linking as well as in any direct use of ld.
LDFLAGS     = -T$(GWT_DEVICE)/ld/$(chip).ld \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtbegin.o \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crti.o \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtn.o \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtend.o


ifeq ($(chip), GAP8)
RISCV_FLAGS     = -mchip=gap8 -mPE=8 -mFC=1 -D__$(chip)__ -D__RISCV_ARCH_GAP__=1
else
RISCV_FLAGS     = -march=rv32imcxpulpv2 -mPE=8 -mFC=1 -D__$(chip)__ -D__RISCV_ARCH_GAP__=1
endif

# Simulation related options
export PULP_CURRENT_CONFIG_ARGS += $(CONFIG_OPT)

# Option to use cluster features
FEATURE_FLAGS   = -DFEATURE_CLUSTER=1

# Option to use preemptive mode
ifeq ($(NO_PREEMPTION), true)
FREERTOS_FLAGS  +=
else
FREERTOS_FLAGS  += -DPREEMPTION
endif

# Deafult is debug bridge
io ?=

# No printf
ifeq ($(io), disable)
FREERTOS_FLAGS  += -D__DISABLE_PRINTF__
endif

# Printf using uart
ifeq ($(io), uart)
FREERTOS_FLAGS  += -DPRINTF_UART
endif

# Printf using stdout
ifeq ($(io), rtl)
FREERTOS_FLAGS  += -DPRINTF_RTL
endif

# Printf using semihosting
ifeq ($(io), host)
export GAP_USE_OPENOCD=1
FREERTOS_FLAGS += -D__SEMIHOSTING__
FREERTOS_FLAGS += -DPRINTF_SEMIHOST
endif

# Enabled for gvsoc
ifeq ($(io), )
FREERTOS_FLAGS  += -DPRINTF_RTL
endif
FREERTOS_FLAGS  += -DGAP_USE_DEBUG_STRUCT


# Simulation platform
# Default is gapuino
# GVSOC
GVSOC_FILES_CLEAN = all_state.txt core_state.txt rt_state.txt efuse_preload.data plt_config.json stimuli tx_uart.log
ifeq ($(platform), gvsoc)
FREERTOS_FLAGS  += -D__PLATFORM_GVSOC__

# FPGA
else ifeq ($(platform), fpga)
FREERTOS_FLAGS  += -D__PLATFORM_FPGA__

# RTL
else ifeq ($(platform), rtl)
FREERTOS_FLAGS  += -D__PLATFORM_RTL__
endif

# Choose Simulator
SIMULATOR      = vsim

ifeq ($(sim), xcelium)
SIMULATOR      = xcelium
endif

# The pre-processor and compiler options.
# Users can override those variables from the command line.
FREERTOS_FLAGS += -D__FREERTOS__=1 -D__PMSIS__=1 -DTOOLCHAIN_GCC_RISCV -DTOOLCHAIN_GCC

COMMON          = -c -fmessage-length=0 -fno-exceptions -fno-builtin \
                  -ffunction-sections -fdata-sections -funsigned-char \
                  -fno-delete-null-pointer-checks -fomit-frame-pointer -O3\
                  $(DEVICE_FLAGS) $(FEATURE_FLAGS) $(RISCV_FLAGS) $(FREERTOS_FLAGS)

PRINTF_FLAGS    = -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL #-DPRINTF_DISABLE_SUPPORT_FLOAT

WARNINGS        = -Wall -Wextra -Wno-unused-parameter -Wno-unused-function \
                  -Wno-unused-variable -Wno-unused-but-set-variable \
                  -Wno-missing-field-initializers -Wno-format -Wimplicit-fallthrough=0

ASMFLAGS        = -x assembler-with-cpp $(COMMON) $(WARNINGS) -DASSEMBLY_LANGUAGE

CFLAGS          = -std=gnu99 $(COMMON) $(WARNINGS) $(PRINTF_FLAGS)

STRIP           = -Wl,--gc-sections,-Map=$@.map,-static #,-s

OBJDUMP_OPT     = -S -D -l -f

# Sources and Includes.
CRT0_SRC        = $(shell find $(GWT_DEVICE) -iname "*.S")
PORT_ASM_SRC    = $(shell find $(PORT_DIR) -iname "*.S")

RTOS_SRC        = $(FREERTOS_SOURCE_DIR)/list.c \
                  $(FREERTOS_SOURCE_DIR)/queue.c \
                  $(FREERTOS_SOURCE_DIR)/tasks.c \
                  $(FREERTOS_SOURCE_DIR)/timers.c \
                  $(FREERTOS_SOURCE_DIR)/event_groups.c \
                  $(FREERTOS_SOURCE_DIR)/stream_buffer.c

PORT_SRC          = $(shell find $(PORT_DIR) -iname "*.c")
DEVICE_SRC        = $(shell find $(GWT_DEVICE) -iname "*.c")
DRIVER_SRC        = $(shell find $(GWT_DRIVER) -iname "*.c")
LIBS_SRC          = $(shell find $(GWT_LIBS)/src -iname "*.c")
PRINTF_SRC        = $(GWT_LIBS)/printf/printf.c

INC_PATH       += . \
                  $(FREERTOS_CONFIG_DIR) \
                  $(PORT_DIR) \
                  $(GWT_TARGET) \
                  $(GWT_DEVICE) \
                  $(GWT_DRIVER) \
                  $(PMSIS_BACKEND)

INC_PATH       += $(FREERTOS_SOURCE_DIR)/include
INC_PATH       += $(GWT_LIBS)/include
INC_PATH       += $(GWT_LIBS)/printf

INCLUDES       += $(foreach f, $(INC_PATH), -I$f)
INCLUDES       += $(FEAT_INCLUDES)

#--- PMSIS drivers ---
PMSIS_SRC         += $(shell find $(GWT_PMSIS) -iname "*.c" ! -path "*gap9*")
PMSIS_BACKEND_SRC  = $(shell find $(PMSIS_BACKEND) -iname "*.c")
PMSIS_INC_PATH    += $(GWT_PMSIS_API)/include/
PMSIS_INC_PATH    += $(shell find $(GWT_PMSIS) -iname "*.h" -not -path "$(GWT_PMSIS)/targets/*" -exec dirname {} \; | uniq)
PMSIS_INC_PATH    += $(GWT_PMSIS)/targets/
PMSIS_INC_PATH    += $(shell find $(GWT_PMSIS)/targets/$(chip) -iname "*.h" -exec dirname {} \; | uniq)
PMSIS_INC_PATH    += $(GWT_PMSIS)/cores
INCLUDES          += $(foreach f, $(PMSIS_INC_PATH), -I$f)

# App sources
DEMO_SRC       += $(FREERTOS_CONFIG_DIR)/FreeRTOS_util.c
APP_SRC        +=
# App includes
INCLUDES       += $(foreach f, $(APP_INC_PATH), -I$f)
# App libs
APP_LIBSFLAGS  +=

# Directory containing built objects
BUILDDIR      = $(shell pwd)/BUILD$(build_dir_ext)/$(TARGET_CHIP)/GCC_RISCV

# Objects
PORT_ASM_OBJ      = $(patsubst %.S, $(BUILDDIR)/%.o, $(PORT_ASM_SRC))
CRT0_OBJ          = $(patsubst %.S, $(BUILDDIR)/%.o, $(CRT0_SRC))
RTOS_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(RTOS_SRC))
PORT_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(PORT_SRC))
DEVICE_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(DEVICE_SRC))
DRIVER_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(DRIVER_SRC))
LIBS_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(LIBS_SRC))
PRINTF_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(PRINTF_SRC))
DEMO_OBJ          = $(patsubst %.c, $(BUILDDIR)/%.o, $(DEMO_SRC))
APP_OBJ           = $(patsubst %.c, $(BUILDDIR)/%.o, $(APP_SRC))
PMSIS_OBJ         = $(patsubst %.c, $(BUILDDIR)/%.o, $(PMSIS_SRC))
PMSIS_BACKEND_OBJ = $(patsubst %.c, $(BUILDDIR)/%.o, $(PMSIS_BACKEND_SRC))

ASM_OBJS        = $(PORT_ASM_OBJ) $(CRT0_OBJ)
C_OBJS          = $(APP_OBJ) $(DEMO_OBJ) $(RTOS_OBJ) $(PORT_OBJ) $(DRIVER_OBJ) \
                  $(DEVICE_OBJ) $(LIBS_OBJ) $(PRINTF_OBJ) \
                  $(API_OBJ) $(HAL_OBJ) $(PMSIS_OBJ) $(PMSIS_BACKEND_OBJ)
OBJS            = $(ASM_OBJS) $(C_OBJS)

BIN             = $(BUILDDIR)/test

OBJS_DUMP       = $(patsubst %.o, %.dump, $(OBJS))


# Makefile targets :
# Build objects (*.o) amd associated dependecies (*.d) with disassembly (*.dump).
#------------------------------------------

all:: dir $(OBJS) $(BIN)

dir:
	mkdir -p $(BUILDDIR)

$(ASM_OBJS): $(BUILDDIR)/%.o: %.S
	@echo "    ASM  $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CC) $(ASMFLAGS) $(INCLUDES) -MD -MF $(basename $@).d -o $@ $<

$(C_OBJS): $(BUILDDIR)/%.o: %.c
	@echo "    CC  $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -MD -MF $(basename $@).d -o $@ $<

$(BIN): $(OBJS)
	@$(CC) -MMD -MP -o $@ $(LDFLAGS) $(OBJS) $(LIBS) $(LIBSFLAGS) $(APP_LIBSFLAGS) $(STRIP)

$(OBJS_DUMP): $(BUILDDIR)/%.dump: $(BUILDDIR)/%.o
	@$(OBJDUMP) $(OBJDUMP_OPT) $< > $@

$(BIN).s: $(BIN)
	@echo "    OBJDUMP  $(shell basename $<) > $(shell basename $@)"
	@$(OBJDUMP) $(OBJDUMP_OPT) $< > $@

ifdef PLPTEST_PLATFORM

run:
	pulp-run --platform $(PLPTEST_PLATFORM) --dir=$(BUILDDIR) --config=$(GVSOC_CONFIG) --binary $(BIN) $(runner_args) prepare run

else

# GVSOC
ifeq ($(platform), gvsoc)
run:: all
	gvsoc --config=$(GVSOC_CONFIG) --dir=$(BUILDDIR) --binary $(BIN) $(runner_args) prepare run
# RTL
else ifeq ($(platform), rtl)
run:: dir
	cd $(BUILDDIR) && $(GAP_SDK_HOME)/tools/runner/run_rtl.sh $(SIMULATOR) $(recordWlf) $(vsimDo) $(vsimPadMuxMode) $(vsimBootTypeMode) $(load) $(PLPBRIDGE_FLAGS) -a $(chip)
# Default : GAPUINO
else
run:: all
ifeq ($(chip), GAP8)
	$(GAP_SDK_HOME)/tools/runner/run_gapuino.sh $(PLPBRIDGE_FLAGS)
else ifeq ($(chip), GAP9)
	$(GAP_SDK_HOME)/tools/runner/run_gap9.sh $(PLPBRIDGE_FLAGS) -ftdi
endif				#ifeq ($(chip), GAP8)
endif				#ifeq ($(platform), )

gdbserver: PLPBRIDGE_FLAGS += -gdb
gdbserver: run
endif

gui:: dir
	cd $(BUILDDIR) && $(GAP_SDK_HOME)/tools/runner/run_rtl.sh $(SIMULATOR) $(recordWlf) $(vsimDo) $(vsimPadMuxMode) $(vsimBootTypeMode) "GUI" $(load) $(PLPBRIDGE_FLAGS) -a $(chip)

# Foramt "vsim -do xxx.do xxx.wlf"
debug:
	@vsim -view $(BUILDDIR)/vsim.wlf "$(vsimDo)"

# Foramt "simvision -input xxx.svcf xxx.trn"
debug_xcelium:
	@simvision "$(vsimDo)" $(BUILDDIR)/waves.shm/waves.trn

disdump: $(OBJS_DUMP) $(BIN).s

version:
	@$(GAP_SDK_HOME)/tools/version/record_version.sh

clean::
	@rm -rf $(OBJS) $(DUMP)
	@rm -rf *~ ./BUILD$(build_dir_ext) transcript *.wav __pycache__
	@rm -rf $(GVSOC_FILES_CLEAN)
	@rm -rf version.log

clean_app::
	@rm -rf $(APP_OBJ) $(BIN) $(OBJS_DUMP)

.PHONY: clean dir all run gui debug version disdump gdbserver clean_app
