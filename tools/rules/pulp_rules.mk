# The C program compiler.
CXX           = riscv32-unknown-elf-g++
CC            = riscv32-unknown-elf-gcc
AR            = riscv32-unknown-elf-ar
OBJDUMP       = riscv32-unknown-elf-objdump

platform     ?= gapuino

# The linker options.
LIBS += -L$(TARGET_INSTALL_DIR)/lib/gap/$(BOARD_NAME) -lpibsp

ifneq (,$(filter $(TARGET_CHIP), GAP8 GAP8_V2))
LIBS          += -L$(TARGET_INSTALL_DIR)/lib/gap \
				-lrt -lrtio -lrt -lgcc
else
LIBS          += -L$(TARGET_INSTALL_DIR)/lib/vega \
				-lrt -lrtio -lrt -lgcc
endif



LIBSFLAGS     += -nostartfiles -nostdlib

# The options used in linking as well as in any direct use of ld.
ifeq ($(TARGET_CHIP), GAP8)
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap8.ld
else ifeq ($(TARGET_CHIP), GAP8_V2)
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap8_rev1.ld
else
LDFLAGS       = -T$(INSTALL_DIR)/ld/link.gap9.ld
endif


ifeq ($(platform), gvsoc)
LDFLAGS       += -T$(INSTALL_DIR)/ld/gvsoc.conf.ld
else ifeq ($(platform), fpga)
LDFLAGS       += -T$(INSTALL_DIR)/ld/fpga.conf.ld
else ifeq ($(platform), rtl)
LDFLAGS       += -T$(INSTALL_DIR)/ld/rtl.conf.ld
else
LDFLAGS       += -T$(INSTALL_DIR)/ld/gapuino.conf.ld
endif

ifneq (,$(filter $(TARGET_CHIP), GAP8 GAP8_V2))
RISCV_FLAGS   = -mchip=gap8 -mPE=8 -mFC=1 -D__riscv__
GAP_FLAGS	 += -D__pulp__ -DCONFIG_GAP -D__PULP_OS__
else
RISCV_FLAGS   = -mchip=gap9 -mPE=8 -mFC=1 -D__riscv__
GAP_FLAGS	 += -D__pulp__ -DCONFIG_VEGA -D__PULP_OS__
endif


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

ifdef GAP_USE_OPENOCD
PULP_CFLAGS += -D__RT_IODEV__=2
endif

TCFLAGS       = -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

# Final binary
#------------------------------------------
BUILDDIR      = $(shell pwd)/BUILD$(build_dir_ext)/$(TARGET_CHIP)/GCC_RISCV

BIN           = $(BUILDDIR)/test

T_OBJECTS_C   = $(patsubst %.c, $(BUILDDIR)/%.o, $(PULP_APP_FC_SRCS) $(PULP_APP_SRCS))

OBJECTS       = $(T_OBJECTS_C)

OBJECTS   += $(BUILDDIR)/pulp-os/conf.o

ifneq (,$(filter $(TARGET_CHIP), GAP8 GAP8_V2))
INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/rt/chips/gap/config.h
else
INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/rt/chips/$(TARGET_NAME)/config.h
endif


INC           = $(TARGET_INSTALL_DIR)/include \
				$(TARGET_INSTALL_DIR)/include/io \
				$(INSTALL_DIR)/include
INC          +=	$(PULP_APP_INC_PATH)

INC_PATH      = $(foreach d, $(INC), -I$d)  $(INC_DEFINE) 


all::    dir $(OBJECTS) $(BIN) disdump

dir:
	mkdir -p $(BUILDDIR)

# Rules for creating dependency files (.d).
#------------------------------------------
$(T_OBJECTS_C) : $(BUILDDIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(BUILDDIR)/pulp-os/conf.o: $(GAP_SDK_HOME)/rtos/pulp/pulp-os/kernel/conf.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(BIN): $(OBJECTS)
	$(CC) -MMD -MP $(WRAP_FLAGS) $(PULP_CFLAGS) $(INC_PATH) -o $(BIN) $(OBJECTS) $(LIBS) $(LDFLAGS) $(LIBSFLAGS) $(INC_DEFINE)

ifdef PLPTEST_PLATFORM
platform=$(PLPTEST_PLATFORM)
ifneq ($(platform), gvsoc)
use_pulprun=1
endif
endif

ifeq ($(use_pulprun), 1)

run:
	pulp-run --platform $(platform) --config=$(GVSOC_CONFIG) --dir=$(BUILDDIR) --binary $(BIN) $(runner_args) prepare run

else ifeq ($(platform), gvsoc)

run::
	gvsoc --config=$(GVSOC_CONFIG) --dir=$(BUILDDIR) --binary $(BIN) $(runner_args) prepare run

else ifeq ($(platform), fpga)
run::
ifneq (,$(filter $(TARGET_CHIP), GAP8 GAP8_V2))
	$(INSTALL_DIR)/runner/run_fpga.sh
else
	$(MBED_PATH)/tools/runner/run_gap9.sh
endif
else ifeq ($(platform), rtl)
run:: dir
	cd $(BUILDDIR) && $(INSTALL_DIR)/runner/run_rtl.sh $(SIMULATOR) $(recordWlf) $(vsimDo) $(vsimPadMuxMode)
else
run:: all
	$(INSTALL_DIR)/runner/run_gapuino.sh $(PLPBRIDGE_FLAGS)

gdbserver: PLPBRIDGE_FLAGS += -gdb
gdbserver: run

endif

flash::
	$(INSTALL_DIR)/runner/run_gapuino.sh -norun $(PLPBRIDGE_FLAGS)

launch::
	$(INSTALL_DIR)/runner/run_gapuino.sh -noflash $(PLPBRIDGE_FLAGS)

gui:: dir
	cd $(BUILDDIR) && $(INSTALL_DIR)/runner/run_rtl.sh $(SIMULATOR) $(recordWlf) $(vsimDo) $(vsimPadMuxMode) "GUI"

# Foramt "vsim -do xxx.do xxx.wlf"
debug:
	@vsim -view $(BUILDDIR)/vsim.wlf "$(vsimDo)"

# Foramt "simvision -input xxx.svcf xxx.trn"
debug_xcelium:
	@simvision "$(vsimDo)" $(BUILDDIR)/waves.shm/waves.trn

$(BIN).s: $(BIN)
	$(OBJDUMP) -D $< > $@

disdump: $(BIN).s

version:
	@$(GAP_SDK_HOME)/tools/version/record_version.sh

clean::
	@rm -rf $(OBJECTS) $(PROGRAM)
	@rm -rf ./BUILD$(build_dir_ext) transcript *.wav __pycache__
	@rm -rf version.log

.PHONY: all clean run debug disdump gdbserver dir
