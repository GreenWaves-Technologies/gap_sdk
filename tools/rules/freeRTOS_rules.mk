# The C program compiler.
CXX         = riscv32-unknown-elf-g++
CC          = riscv32-unknown-elf-gcc
AR          = riscv32-unknown-elf-ar
OBJDUMP     = riscv32-unknown-elf-objdump

ifndef chip
chip=GAP8
endif

# Directories
FREERTOS_CONFIG_DIR = $(FREERTOS_PATH)/demos/gwt/gap8/common/config_files
FREERTOS_SOURCE_DIR = $(FREERTOS_PATH)/lib/FreeRTOS
PORT_DIR            = $(FREERTOS_SOURCE_DIR)/portable/GCC/GAP8
GWT_PACK_DIR        = $(FREERTOS_PATH)/lib/third_party/mcu_vendor/gwt
GWT_CMSIS           = $(GWT_PACK_DIR)/cmsis/TARGET_RISCV_32
GWT_FEAT            = $(GWT_PACK_DIR)/features
GWT_TARGET          = $(GWT_PACK_DIR)/TARGET_GWT
GWT_DEVICE          = $(GWT_TARGET)/TARGET_$(chip)/device
GWT_DRIVER          = $(GWT_TARGET)/TARGET_$(chip)/driver
GWT_PINS            = $(GWT_TARGET)/TARGET_$(chip)/pins

# The linker options.
LIBS            = -L$(GWT_TARGET)/libs -L$(GWT_TARGET)/libs/newlib

LIBSFLAGS       = -lc -lm -lgcc -nostartfiles

# The options used in linking as well as in any direct use of ld.
LDFLAGS     = -T$(GWT_DEVICE)/ld/$(chip).ld \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtbegin.o \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crti.o \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtn.o \
              $(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtend.o


ifeq ($(chip), GAP8)
RISCV_FLAGS     = -march=rv32imcxgap8 -mPE=8 -mFC=1 -D__$(chip)__
else
RISCV_FLAGS     = -march=rv32imcxpulpv2 -mPE=8 -mFC=1 -D__$(chip)__
endif

DEVICE_FLAGS    = -DDEVICE_SPI_ASYNCH=1 -DDEVICE_SPI=1 \
                  -DDEVICE_SERIAL=1 -DDEVICE_SERIAL_ASYNCH=1 \
                  -DDEVICE_HYPERBUS_ASYNCH=1 -DDEVICE_HYPERBUS=1 \
                  -DDEVICE_STDIO_MESSAGES=1 -DDEVICE_SLEEP=1 \
                  -DDEVICE_PORTIN=1 -DDEVICE_PORTOUT=1 -DDEVICE_PORTINOUT=1 \
                  -DDEVICE_I2C=1 -DDEVICE_I2C_ASYNCH=1 -DDEVICE_I2S=1 -DDEVICE_RTC=1 \
                  -DDEVICE_INTERRUPTIN=1 -DDEVICE_PWMOUT=1

# User can choose use cluster
FEATURE_FLAGS   = -DFEATURE_CLUSTER=1

FREERTOS_FLAGS  += -D__FREERTOS__=1 -DTOOLCHAIN_GCC_RISCV -DTOOLCHAIN_GCC

ifeq ($(NO_PREEMPTION), true)
FREERTOS_FLAGS  +=
else
FREERTOS_FLAGS  += -DPREEMPTION
endif

# The pre-processor and compiler options.
# Users can override those variables from the command line.

COMMON      = -c -fmessage-length=0 -fno-exceptions -fno-builtin \
              -ffunction-sections -fdata-sections -funsigned-char \
              -fno-delete-null-pointer-checks -fomit-frame-pointer -Os \
              $(DEVICE_FLAGS) $(FEATURE_FLAGS) $(RISCV_FLAGS) $(FREERTOS_FLAGS)

WARNINGS    = -Wall -Wextra -Wno-unused-parameter -Wno-unused-function \
              -Wno-unused-variable -Wno-unused-but-set-variable \
              -Wno-missing-field-initializers -Wno-format

ASMFLAGS    = -x assembler-with-cpp $(COMMON) $(WARNINGS)

CFLAGS      = -std=gnu99 $(COMMON) $(WARNINGS)

STRIP       = -Wl,--gc-sections -Wl,-Map=$@.map -Wl,-static #-Wl,-s

# Sources and Includes.
CRT0_SRC        = $(shell find $(GWT_DEVICE) -iname "*.S")
PORT_ASM_SRC    = $(shell find $(PORT_DIR) -iname "*.S")

RTOS_SRC        = $(FREERTOS_SOURCE_DIR)/list.c \
                  $(FREERTOS_SOURCE_DIR)/queue.c \
                  $(FREERTOS_SOURCE_DIR)/tasks.c \
                  $(FREERTOS_SOURCE_DIR)/timers.c \
                  $(FREERTOS_SOURCE_DIR)/event_groups.c \
                  $(FREERTOS_SOURCE_DIR)/portable/MemMang/heap_5.c \
                  $(FREERTOS_SOURCE_DIR)/stream_buffer.c

PORT_SRC        = $(shell find $(PORT_DIR) -iname "*.c")
FEAT_SRC        = $(shell find $(GWT_FEAT) -iname "*.c")
DEVICE_SRC      = $(shell find $(GWT_DEVICE) -iname "*.c")
DRIVER_SRC      = $(shell find $(GWT_DRIVER) -iname "*.c")
PINS_SRC        = $(shell find $(GWT_PINS) -iname "*.c")

FEAT_INCLUDES   = $(foreach f, $(shell find $(GWT_FEAT) -iname "*.h" -exec dirname {} \;), -I$f)

INC_PATH       += . \
                  $(FREERTOS_SOURCE_DIR)/../include \
                  $(FREERTOS_SOURCE_DIR)/../include/private \
                  $(FREERTOS_CONFIG_DIR) \
                  $(PORT_DIR) \
                  $(GWT_CMSIS) \
                  $(GWT_TARGET)/libs/newlib/extra/stdio/tinyprintf \
                  $(GWT_DEVICE) \
                  $(GWT_DRIVER) \
                  $(GWT_PINS) \
                  $(TARGET_INSTALL_DIR)/include

INCLUDES        = $(foreach f, $(INC_PATH), -I$f)
INCLUDES       += $(FEAT_INCLUDES)

# App sources
DEMO_SRC       += $(FREERTOS_CONFIG_DIR)/FreeRTOS_util.c
APP_SRC        +=

# Directory containing built objects
BUILDDIR        = $(shell pwd)/BUILD/$(chip)/GCC_RISCV

# Objects
PORT_ASM_OBJ    = $(patsubst %.S, $(BUILDDIR)/%.o, $(PORT_ASM_SRC))
CRT0_OBJ        = $(patsubst %.S, $(BUILDDIR)/%.o, $(CRT0_SRC))
RTOS_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(RTOS_SRC))
PORT_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(PORT_SRC))
DRIVER_OBJ      = $(patsubst %.c, $(BUILDDIR)/%.o, $(DRIVER_SRC))
PINS_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(PINS_SRC))
FEAT_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(FEAT_SRC))
DEVICE_OBJ      = $(patsubst %.c, $(BUILDDIR)/%.o, $(DEVICE_SRC))
DEMO_OBJ        = $(patsubst %.c, $(BUILDDIR)/%.o, $(DEMO_SRC))
APP_OBJ         = $(patsubst %.c, $(BUILDDIR)/%.o, $(APP_SRC))

ASM_OBJS        = $(PORT_ASM_OBJ) $(CRT0_OBJ)
C_OBJS          = $(APP_OBJ) $(DEMO_OBJ) $(RTOS_OBJ) $(PORT_OBJ) $(DRIVER_OBJ) \
                  $(FEAT_OBJ) $(PINS_OBJ) $(DEVICE_OBJ)

OBJS            = $(ASM_OBJS) $(C_OBJS)


BIN             = $(BUILDDIR)/test

all::   dir $(OBJS) $(BIN) version disdump

dir:
	mkdir -p $(BUILDDIR)

# Rules for creating dependency files (.d).
#------------------------------------------

$(ASM_OBJS): $(BUILDDIR)/%.o: %.S
	@echo "    SS  $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CC) $(ASMFLAGS) -o $@ $<


$(C_OBJS): $(BUILDDIR)/%.o: %.c
	@echo "    CC  $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<
	@$(OBJDUMP) -S -d $@ > $(patsubst %.o, %.dump, $@)

$(BIN): $(OBJS)
	@$(CC) -MMD -MP -o $@ $(LDFLAGS) $(OBJS) $(LIBS) $(LIBSFLAGS) $(STRIP)

ifdef gvsoc
run::
	$(GAP_SDK_HOME)/tools/runner/run_gvsoc.sh
else ifdef fpga
run::
	$(GAP_SDK_HOME)/tools/runner/run_fpga.sh
else ifdef rtl
run:: dir
	@ln -sf $(VSIM_PATH)/work $(BUILDDIR)/work
	@ln -sf $(VSIM_PATH)/modelsim.ini $(BUILDDIR)/modelsim.ini
	@ln -sf $(VSIM_PATH)/tcl_files $(BUILDDIR)/tcl_files
	@ln -sf $(VSIM_PATH)/boot $(BUILDDIR)/boot
	cd $(BUILDDIR) && $(GAP_SDK_HOME)/tools/runner/run_rtl.sh $(recordWlf) $(vsimDo) $(vsimPadMuxMode) $(vsimBootTypeMode)
else
run:: all
	$(GAP_SDK_HOME)/tools/runner/run_gapuino.sh $(PLPBRIDGE_FLAGS)

gdbserver: PLPBRIDGE_FLAGS += -gdb
gdbserver: run

endif

gui:: dir
	@ln -sf $(VSIM_PATH)/work $(BUILDDIR)/work
	@ln -sf $(VSIM_PATH)/modelsim.ini $(BUILDDIR)/modelsim.ini
	@ln -sf $(VSIM_PATH)/tcl_files $(BUILDDIR)/tcl_files
	@ln -sf $(VSIM_PATH)/boot $(BUILDDIR)/boot
	cd $(BUILDDIR) && $(MBED_PATH)/tools/runner/run_rtl.sh $(recordWlf) $(vsimDo) $(vsimPadMuxMode) $(vsimBootTypeMode) "GUI" $(PLPBRIDGE_FLAGS)

debug:
	@vsim -view $(BUILDDIR)/gap.wlf "$(vsimDo)"

$(BIN).s: $(BIN)
	$(OBJDUMP) -D $< > $@

disdump: $(BIN).s

version:
	@$(MBED_PATH)/tools/version/record_version.sh

clean::
	@rm -rf $(OBJS) $(DUMP) $(TEST_OBJ)
	@rm -rf *~ ./BUILD transcript *.wav __pycache__
	@rm -rf version.log
