# The C program compiler.
CXX           = riscv32-unknown-elf-g++
CC            = riscv32-unknown-elf-gcc
AR            = riscv32-unknown-elf-ar
OBJDUMP       = riscv32-unknown-elf-objdump

# The linker options.
LIBS          += -L$(TARGET_INSTALL_DIR)/libs \
				-lrt -lio -lrt -lgcc

LIBSFLAGS     += -nostartfiles -nostdlib

# The options used in linking as well as in any direct use of ld.
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap8.ld

ifdef gvsoc
LDFLAGS       += -T$(INSTALL_DIR)/ld/gvsoc.conf.ld
else ifdef fpga
LDFLAGS       += -T$(INSTALL_DIR)/ld/fpga.conf.ld
else ifdef rtl
LDFLAGS       += -T$(INSTALL_DIR)/ld/rtl.conf.ld
else
LDFLAGS       += -T$(INSTALL_DIR)/ld/gapuino.conf.ld
endif

RISCV_FLAGS   = -march=rv32imcxgap8 -mPE=8 -mFC=1 -D__riscv__

WRAP_FLAGS    = -Wl,--gc-sections

GAP_FLAGS	 += -D__pulp__

# The pre-processor and compiler options.
# Users can override those variables from the command line.
COMMON        = -Os -g -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS)

ASMFLAGS      = $(COMMON) -DLANGUAGE_ASSEMBLY -MMD -MP -c

BOOTFLAGS	  = -Os -g -DUSE_AES -fno-jump-tables -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

CFLAGS        = $(COMMON) -MMD -MP -c

TCFLAGS       = -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

# Final binary
#------------------------------------------
BUILDDIR      = $(shell pwd)/BUILD/GAP8/GCC_RISCV

BIN           = $(BUILDDIR)/test

T_OBJECTS_C   = $(patsubst %.c, $(BUILDDIR)/%.o, $(PULP_APP_FC_SRCS) $(PULP_APP_SRCS))

OBJECTS       = $(T_OBJECTS_C)

INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/pulp-os/gap_config.h

INC           = $(TARGET_INSTALL_DIR)/include/pulp-os \
				$(TARGET_INSTALL_DIR)/include/pulp-os/io \
				$(GAP_SDK_HOME)/tools/pulp-debug-bridge/include

INC_PATH      = $(foreach d, $(INC), -I$d)  $(INC_DEFINE)

all::    dir $(OBJECTS) $(BIN) disdump

dir:
	mkdir -p $(BUILDDIR)
	ln -sf $(VSIM_PATH)/work $(BUILDDIR)/work
	ln -sf $(VSIM_PATH)/modelsim.ini $(BUILDDIR)/modelsim.ini
	ln -sf $(VSIM_PATH)/tcl_files $(BUILDDIR)/tcl_files

# Rules for creating dependency files (.d).
#------------------------------------------
$(T_OBJECTS_C) : $(BUILDDIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(BIN): $(OBJECTS)
	$(CC) -MMD -MP $(WRAP_FLAGS) $(PULP_CFLAGS) -o $(BIN) $(OBJECTS) $(LIBS) $(LDFLAGS) $(LIBSFLAGS) $(INC_DEFINE)

ifdef gvsoc
run::
	$(INSTALL_DIR)/runner/run_gvsoc.sh
else ifdef fpga
run::
	$(INSTALL_DIR)/runner/run_fpga.sh
else ifdef rtl
run::
	@ln -sf $(VSIM_PATH)/boot $(BUILDDIR)/boot
	cd $(BUILDDIR) && $(INSTALL_DIR)/runner/run_rtl.sh $(recordWlf) $(vsimDo) $(vsimPadMuxMode)
else
run:: all
	$(INSTALL_DIR)/runner/run_gapuino.sh $(PLPBRIDGE_FLAGS)

gdbserver: PLPBRIDGE_FLAGS=-gdb
gdbserver: run
endif

gui:
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
