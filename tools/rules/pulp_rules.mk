# The C program compiler.
CXX           = riscv32-unknown-elf-g++
CC            = riscv32-unknown-elf-gcc
AR            = riscv32-unknown-elf-ar
OBJDUMP       = riscv32-unknown-elf-objdump

platform     ?= gapuino
vcd          ?= ""
_vcd         ?=
trace	     ?= ""
_trace       ?=
config       ?= ""
_config      ?=

# The linker options.
LIBS          += -L$(TARGET_INSTALL_DIR)/lib/gap \
				-lrt -lrtio -lrt -lgcc

LIBSFLAGS     += -nostartfiles -nostdlib

# The options used in linking as well as in any direct use of ld.
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap8.ld

ifeq ($(platform), gvsoc)
LDFLAGS       += -T$(INSTALL_DIR)/ld/gvsoc.conf.ld
else ifeq ($(platform), fpga)
LDFLAGS       += -T$(INSTALL_DIR)/ld/fpga.conf.ld
else ifeq ($(platform), rtl)
LDFLAGS       += -T$(INSTALL_DIR)/ld/rtl.conf.ld
else
LDFLAGS       += -T$(INSTALL_DIR)/ld/gapuino.conf.ld
endif

RISCV_FLAGS   = -march=rv32imcxgap8 -mPE=8 -mFC=1 -D__riscv__

WRAP_FLAGS    = -Wl,--gc-sections

GAP_FLAGS	 += -D__pulp__ -DCONFIG_GAP

# The pre-processor and compiler options.
# Users can override those variables from the command line.
COMMON        = -Os -g -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS)

ASMFLAGS      = $(COMMON) -DLANGUAGE_ASSEMBLY -MMD -MP -c

BOOTFLAGS	  = -Os -g -DUSE_AES -fno-jump-tables -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

CFLAGS        = $(COMMON) -MMD -MP -c

TCFLAGS       = -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

# Final binary
#------------------------------------------
BUILDDIR      = $(shell pwd)/BUILD/$(TARGET_CHIP)/GCC_RISCV

BIN           = $(BUILDDIR)/test

T_OBJECTS_C   = $(patsubst %.c, $(BUILDDIR)/%.o, $(PULP_APP_FC_SRCS) $(PULP_APP_SRCS))

OBJECTS       = $(T_OBJECTS_C)

INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/$(TARGET_NAME)_config.h

INC           = $(TARGET_INSTALL_DIR)/include \
				$(TARGET_INSTALL_DIR)/include/io \
				$(INSTALL_DIR)/include

INC_PATH      = $(foreach d, $(INC), -I$d)  $(INC_DEFINE)

all::    dir $(OBJECTS) $(BIN) disdump

dir:
	mkdir -p $(BUILDDIR)

# Rules for creating dependency files (.d).
#------------------------------------------
$(T_OBJECTS_C) : $(BUILDDIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(BIN): $(OBJECTS)
	$(CC) -MMD -MP $(WRAP_FLAGS) $(PULP_CFLAGS) -o $(BIN) $(OBJECTS) $(LIBS) $(LDFLAGS) $(LIBSFLAGS) $(INC_DEFINE)

ifeq ($(platform), gvsoc)

ifneq ($(vcd), "")
_vcd=-vcd
endif

ifneq ($(trace), "")
_trace=-trace $(trace)
endif

ifneq ($(config), "")
_config=-config ${CURDIR}/$(config)
endif

run::
	$(INSTALL_DIR)/runner/run_gvsoc.sh $(_config) $(_vcd) $(_trace)

else ifeq ($(platform), fpga)
run::
	$(INSTALL_DIR)/runner/run_fpga.sh
else ifeq ($(platform), rtl)
run:: dir
	@ln -sf $(VSIM_PATH)/work $(BUILDDIR)/work
	@ln -sf $(VSIM_PATH)/modelsim.ini $(BUILDDIR)/modelsim.ini
	@ln -sf $(VSIM_PATH)/tcl_files $(BUILDDIR)/tcl_files
	@ln -sf $(VSIM_PATH)/boot $(BUILDDIR)/boot
	cd $(BUILDDIR) && $(INSTALL_DIR)/runner/run_rtl.sh $(recordWlf) $(vsimDo) $(vsimPadMuxMode)
else
run:: all
	$(INSTALL_DIR)/runner/run_gapuino.sh $(PLPBRIDGE_FLAGS)

gdbserver: PLPBRIDGE_FLAGS += -gdb
gdbserver: run

endif

gui:: dir
	@ln -sf $(VSIM_PATH)/work $(BUILDDIR)/work
	@ln -sf $(VSIM_PATH)/modelsim.ini $(BUILDDIR)/modelsim.ini
	@ln -sf $(VSIM_PATH)/tcl_files $(BUILDDIR)/tcl_files
	@ln -sf $(VSIM_PATH)/boot $(BUILDDIR)/boot
	cd $(BUILDDIR) && $(INSTALL_DIR)/runner/run_rtl.sh $(recordWlf) $(vsimDo) $(vsimPadMuxMode) "GUI"

debug:
	@vsim -view $(BUILDDIR)/gap.wlf "$(vsimDo)"

$(BIN).s: $(BIN)
	$(OBJDUMP) -D $< > $@

disdump: $(BIN).s

clean::
	@rm -rf $(OBJECTS) $(PROGRAM)
	@rm -rf ./BUILD transcript *.wav __pycache__

.PHONY: all clean run debug disdump gdbserver dir
