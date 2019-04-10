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
LIBS          += -L$(MBED_PATH)/mbed-os/targets/TARGET_GWT/libs  \
		-L$(MBED_PATH)/mbed-os/targets/TARGET_GWT/libs/newlib
LIBSFLAGS     += -nostartfiles -lgcc -lc -lm

ifndef chip
chip=GAP8
endif

# The options used in linking as well as in any direct use of ld.
LDFLAGS       = -T$(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_$(chip)/device/ld/$(chip).ld \
		$(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtbegin.o \
		$(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crti.o \
		$(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtn.o \
		$(GAP_RISCV_GCC_TOOLCHAIN)/lib/gcc/riscv32-unknown-elf/7.1.1/crtend.o \
		-lstdc++

ifeq ($(chip), GAP8)
RISCV_FLAGS   = -march=rv32imcxgap8 -mPE=8 -mFC=1 -D__$(chip)__  -D__RISCV_ARCH_GAP__=1
else
RISCV_FLAGS   = -march=rv32imcxpulpv2 -mPE=8 -mFC=1 -D__$(chip)__ -D__RISCV_ARCH_GAP__=1
endif

DEVICE_FLAGS  = -DDEVICE_SPI_ASYNCH=1 -DDEVICE_SPI=1 \
		-DDEVICE_SERIAL=1 -DDEVICE_SERIAL_ASYNCH=1 \
		-DDEVICE_HYPERBUS_ASYNCH=1 -DDEVICE_HYPERBUS=1 \
		-DDEVICE_STDIO_MESSAGES=1 -DDEVICE_SLEEP=1 \
		-DDEVICE_PORTIN=1 -DDEVICE_PORTOUT=1 -DDEVICE_PORTINOUT=1 \
		-DDEVICE_I2C=1 -DDEVICE_I2C_ASYNCH=1 -DDEVICE_I2S=1 -DDEVICE_RTC=1 \
		-DDEVICE_INTERRUPTIN=1 -DDEVICE_PWMOUT=1 -DDEVICE_QSPI=1



WRAP_FLAGS    = -Wl,--gc-sections -Wl,--wrap,main -Wl,--wrap,_malloc_r -Wl,--wrap,_free_r -Wl,--wrap,_realloc_r \
		-Wl,--wrap,_memalign_r -Wl,--wrap,_calloc_r -Wl,--wrap,atexit -Wl,-n

# User can choose use cluster
FEATURE_FLAGS = -DFEATURE_CLUSTER=1

# User can choose use Mbed OS
MBED_FLAGS    += -D__MBED__=1 -DTOOLCHAIN_GCC_RISCV -DTOOLCHAIN_GCC

# For GVSOC platform simulation
ifeq ($(platform), gvsoc)
export PULP_CURRENT_CONFIG_ARGS += $(CONFIG_OPT)
MBED_FLAGS     += -D__PLATFORM_GVSOC__
endif

ifeq ($(platform), fpga)
MBED_FLAGS     += -D__PLATFORM_FPGA__
endif

ifeq ($(platform), rtl)
MBED_FLAGS     += -D__PLATFORM_RTL__
endif

ifdef no_printf
MBED_FLAGS     += -D__DISABLE_PRINTF__
else
MBED_FLAGS     += -DPRINTF_RTL
endif

ifeq ($(platform), gapuino)
export PULP_CURRENT_CONFIG_ARGS += $(CONFIG_OPT)
endif

# The pre-processor and compiler options.
# Users can override those variables from the command line.
COMMON        = -c -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -fmessage-length=0 -fno-exceptions -fno-builtin -ffunction-sections -fdata-sections -funsigned-char -MMD -fno-delete-null-pointer-checks -fomit-frame-pointer -Wno-deprecated-declarations -Os -g1 $(DEVICE_FLAGS) $(FEATURE_FLAGS) $(RISCV_FLAGS) $(MBED_FLAGS)

ASMFLAGS      = -x assembler-with-cpp $(COMMON)

CFLAGS        = -std=gnu99 $(COMMON)

CXXFLAGS      = -std=gnu++98 -fno-rtti -Wvla $(COMMON)

# Final binary
#------------------------------------------
BIN           = $(BUILDDIR)/test

BUILDDIR      = $(shell pwd)/BUILD/$(TARGET_CHIP)/GCC_RISCV

S_OBJECTS     = $(patsubst %.S, $(BUILDDIR)/%.o, $(wildcard $(shell find $(MBED_PATH)/mbed-os -name "*.S" \
		-not -path "$(MBED_PATH)/mbed-os/targets/TARGET_CORTEX/*" \
		-not -path "$(MBED_PATH)/mbed-os/rtos/TARGET_CORTEX/*" \
		-not -path "$(MBED_PATH)/mbed-os/cmsis/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/*" \
		-not -path "$(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_*")))

S_OBJECTS     += $(patsubst %.S, $(BUILDDIR)/%.o, $(wildcard $(shell find $(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_$(chip) -name "*.S")))

C_OBJECTS     = $(patsubst %.c, $(BUILDDIR)/%.o, $(wildcard $(shell find $(MBED_PATH)/mbed-os -name "*.c" \
		-not -path "$(MBED_PATH)/mbed-os/components/*" \
		-not -path "$(MBED_PATH)/mbed-os/targets/TARGET_GWT/libs/*" \
		-not -path "$(MBED_PATH)/mbed-os/events/equeue/tests/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/nfc/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/FEATURE_BLE/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-client-randlib/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-trace/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-client-cli/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/nanostack-libservice/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-coap/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/lorawan/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/lwipstack/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/mbedtls/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/nanostack/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/netsocket/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/nfc/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/cellular/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/cryptocell/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/device_key/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/FEATURE_STORAGE/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/nvstore/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/system_storage/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/TESTS/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs/littlefs/test/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs/littlefs/emubd/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/kvstore/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/nvstore/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/unsupported/*" \
		-not -path "$(MBED_PATH)/mbed-os/hal/TARGET_FLASH_CMSIS_ALGO/*"  \
		-not -path "$(MBED_PATH)/mbed-os/hal/storage_abstraction/*"  \
		-not -path "$(MBED_PATH)/mbed-os/rtos/TARGET_CORTEX/*"  \
		-not -path "$(MBED_PATH)/mbed-os/tools/*"  \
		-not -path "$(MBED_PATH)/mbed-os/TEST_APPS/*"  \
		-not -path "$(MBED_PATH)/mbed-os/TESTS/*"  \
		-not -path "$(MBED_PATH)/mbed-os/UNITTESTS/*"  \
		-not -path "$(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_*")))

C_OBJECTS     += $(patsubst %.c, $(BUILDDIR)/%.o, $(wildcard $(shell find $(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_$(chip) -name "*.c")))

T_OBJECTS_C   += $(patsubst %.c, $(BUILDDIR)/%.o, $(TEST_C))

T_OBJECTS_CXX += $(patsubst %.cpp, $(BUILDDIR)/%.o, $(TEST_CXX))

CXX_OBJECTS   = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(wildcard $(shell find $(MBED_PATH)/mbed-os -name "*.cpp" \
		-not -path "$(MBED_PATH)/mbed-os/tools/*" \
		-not -path "$(MBED_PATH)/mbed-os/components/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/FEATURE_BLE/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-client-randlib/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-trace/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-client-cli/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/nanostack-libservice/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/mbed-coap/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/lorawan/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/mbedtls/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/lwipstack/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/nanostack/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/netsocket/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/nfc/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/cellular/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/cryptocell/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/device_key/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/FEATURE_STORAGE/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/nvstore/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/system_storage/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/TESTS/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs/TESTS/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs/TESTS_COMMON/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs/littlefs/test/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs/littlefs/emubd/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/kvstore/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/storage/nvstore/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/unsupported/*" \
		-not -path "$(MBED_PATH)/mbed-os/hal/TARGET_FLASH_CMSIS_ALGO/*"  \
		-not -path "$(MBED_PATH)/mbed-os/hal/storage_abstraction/*"  \
		-not -path "$(MBED_PATH)/mbed-os/rtos/TARGET_CORTEX/*"  \
		-not -path "$(MBED_PATH)/mbed-os/tools/*"  \
		-not -path "$(MBED_PATH)/mbed-os/TEST_APPS/*"  \
		-not -path "$(MBED_PATH)/mbed-os/TESTS/*"  \
		-not -path "$(MBED_PATH)/mbed-os/UNITTESTS/*"  \
		-not -path "$(MBED_PATH)/mbed-os/features/frameworks/utest/TESTS/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/spif-driver/TESTS/*" \
		-not -path "$(MBED_PATH)/mbed-os/features/i2cee-driver/TESTS/*")))

OBJECTS       = $(S_OBJECTS) $(T_OBJECTS_C) $(T_OBJECTS_CXX) $(C_OBJECTS) $(CXX_OBJECTS)

INC_DEFINE    = -include $(MBED_PATH)/mbed-os/targets/TARGET_GWT/mbed_config.h

INC           += $(GAP_SDK_HOME)/tools/libs $(MBED_PATH)/mbed-os/ $(MBED_PATH)/mbed-os/hal/ $(MBED_PATH)/mbed-os/platform/ $(MBED_PATH)/mbed-os/cmsis \
		$(MBED_PATH)/mbed-os/cmsis/TARGET_RISCV_32 $(MBED_PATH)/mbed-os/rtos \
		$(MBED_PATH)/mbed-os/rtos/TARGET_RISCV/ $(MBED_PATH)/mbed-os/rtos/TARGET_RISCV/rtx4/ $(MBED_PATH)/mbed-os/rtos/TARGET_RISCV/rtx5/ \
		$(MBED_PATH)/mbed-os/rtos/TARGET_RISCV/rtx5/Include  $(MBED_PATH)/mbed-os/rtos/TARGET_RISCV/rtx5/RTX/Config \
		$(MBED_PATH)/mbed-os/rtos/TARGET_RISCV/rtx5/RTX/Include  $(MBED_PATH)/mbed-os/rtos/TARGET_RISCV/rtx5/RTX/Source \
		$(MBED_PATH)/mbed-os/targets/TARGET_GWT/api/ $(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_$(chip)/ \
		$(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_$(chip)/device/ \
		$(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_$(chip)/driver/ $(MBED_PATH)/mbed-os/targets/TARGET_GWT/TARGET_$(chip)/pins/ \
		$(MBED_PATH)/mbed-os/targets/TARGET_GWT/libs/newlib/extra/stdio/tinyprintf/ \
		$(MBED_PATH)/mbed-os/events/ $(MBED_PATH)/mbed-os/events/equeue/ \
		$(MBED_PATH)/mbed-os/drivers $(MBED_PATH)/mbed-os/features \
		$(MBED_PATH)/mbed-os/features/storage $(MBED_PATH)/mbed-os/features/storage/blockdevice \
		$(MBED_PATH)/mbed-os/features/storage/filesystem $(MBED_PATH)/mbed-os/features/storage/filesystem/bd \
		$(MBED_PATH)/mbed-os/features/storage/filesystem/fat $(MBED_PATH)/mbed-os/features/storage/filesystem/fat/ChaN \
		$(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs $(MBED_PATH)/mbed-os/features/storage/filesystem/littlefs/littlefs \
		$(MBED_PATH)/mbed-os/features/frameworks/greentea-client $(MBED_PATH)/mbed-os/features/frameworks/greentea-client/greentea-client \
		$(MBED_PATH)/mbed-os/features/frameworks/unity $(MBED_PATH)/mbed-os/features/frameworks/unity/unity \
		$(MBED_PATH)/mbed-os/features/frameworks/utest $(MBED_PATH)/mbed-os/features/frameworks/utest/utest \
		$(MBED_PATH)/mbed-os/features/mbedtls \
		$(MBED_PATH)/mbed-os/features/mbedtls/inc \
		$(MBED_PATH)/mbed-os/features/spif-driver \
		$(MBED_PATH)/mbed-os/features/i2cee-driver \
		$(MBED_PATH)/mbed-os/features/hyperbus-driver \
		$(MBED_PATH)/mbed-os/features/FEATURE_CLUSTER\
		$(MBED_PATH)/mbed-os/features/storage/filesystem/rofs \
		$(TARGET_INSTALL_DIR)/include


INC_PATH      += $(foreach d, $(INC), -I$d)  $(INC_DEFINE)

all:: $(OBJECTS) $(BIN) disdump

dir:
	mkdir -p $(BUILDDIR)

# Rules for creating dependency files (.d).
#------------------------------------------
$(S_OBJECTS) : $(BUILDDIR)/%.o : %.S
	@echo "    SS  $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CC) $(ASMFLAGS) $(INC_PATH) -o $@ $<

$(C_OBJECTS) : $(BUILDDIR)/%.o : %.c
	@echo "    CC  $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC_PATH) -MD -MF $(basename $@).d -o $@ $<

$(T_OBJECTS_C) : $(BUILDDIR)/%.o : %.c
	@echo "    CC  $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC_PATH) -MD -MF $(basename $@).d -o $@ $<

$(T_OBJECTS_CXX) : $(BUILDDIR)/%.o : %.cpp
	@echo "    CXX $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INC_PATH) -MD -MF $(basename $@).d -o $@ $<

$(CXX_OBJECTS) : $(BUILDDIR)/%.o : %.cpp
	@echo "    CXX $(shell basename $<)"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INC_PATH) -MD -MF $(basename $@).d -o $@ $<

$(BIN): $(OBJECTS)
	@$(CC) $(RISCV_FLAGS) -MMD -MP $(WRAP_FLAGS) -o $@ $(OBJECTS) $(LIBS) $(LDFLAGS) $(LIBSFLAGS) $(INC_DEFINE)

ifeq ($(platform), gvsoc)
run::
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

else ifeq ($(platform), rtl)
run:: dir
	@ln -sf $(VSIM_PATH)/work $(BUILDDIR)/work
	@ln -sf $(VSIM_PATH)/modelsim.ini $(BUILDDIR)/modelsim.ini
	@ln -sf $(VSIM_PATH)/tcl_files $(BUILDDIR)/tcl_files
	@ln -sf $(VSIM_PATH)/boot $(BUILDDIR)/boot
	@ln -sf $(VSIM_PATH)/../tb/models $(BUILDDIR)/models
	cd $(BUILDDIR) && $(MBED_PATH)/tools/runner/run_rtl.sh $(recordWlf) $(vsimDo) $(vsimPadMuxMode) $(vsimBootTypeMode) $(load) $(PLPBRIDGE_FLAGS) -a $(chip)
else
run:: all
	$(MBED_PATH)/tools/runner/run_gapuino.sh $(PLPBRIDGE_FLAGS)

gdbserver: PLPBRIDGE_FLAGS += -gdb
gdbserver: run

endif
gui:: dir
	@ln -sf $(VSIM_PATH)/work $(BUILDDIR)/work
	@ln -sf $(VSIM_PATH)/modelsim.ini $(BUILDDIR)/modelsim.ini
	@ln -sf $(VSIM_PATH)/tcl_files $(BUILDDIR)/tcl_files
	@ln -sf $(VSIM_PATH)/boot $(BUILDDIR)/boot
	@ln -sf $(VSIM_PATH)/../tb/models $(BUILDDIR)/models
	cd $(BUILDDIR) && $(MBED_PATH)/tools/runner/run_rtl.sh $(recordWlf) $(vsimDo) $(vsimPadMuxMode) $(vsimBootTypeMode) "GUI" $(load) $(PLPBRIDGE_FLAGS) -a $(chip)

debug:
	@vsim -view $(BUILDDIR)/vsim.wlf "$(vsimDo)"

$(BUILDDIR)/test.s: $(BUILDDIR)/test
	$(OBJDUMP) -D $< > $@

disdump: $(BUILDDIR)/test.s


clean::
	@rm -rf $(OBJECTS) $(PROGRAM)
	@rm -rf ./BUILD transcript *.wav __pycache__

.PHONY: gui debug disdump clean gdbserver run all dir
