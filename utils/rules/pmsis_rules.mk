BOARD_NAME ?= gapuino

ifeq ($(TARGET_CHIP_FAMILY), GAP9)
PMSIS_OS ?= freertos
else
PMSIS_OS ?=pulpos
endif				# TARGET_CHIP_FAMILY

ifndef platform
ifdef PMSIS_PLATFORM
platform = $(PMSIS_PLATFORM)
else
platform = board
endif				# platform
endif				# PMSIS_PLATFORM

SHELL=bash
ECHO_GREEN = $(shell tput setaf 2)
ECHO_BOLD = $(shell tput bold)
ECHO_CLEAR = $(shell tput sgr0)

help:
	@echo "=================== ${ECHO_BOLD}${ECHO_GREEN}GAP SDK Application${ECHO_CLEAR} ==================="
	@echo ""
	@echo "Main targets:"
	@echo " - ${ECHO_BOLD}clean${ECHO_CLEAR} : clean the application"
	@echo " - ${ECHO_BOLD}all${ECHO_CLEAR}   : build the application"
	@echo " - ${ECHO_BOLD}run${ECHO_CLEAR}   : run the application"
	@echo ""
	@echo "Common options:"
	@echo " - ${ECHO_BOLD}platform=<value>${ECHO_CLEAR} : select the platform (gvsoc or board)"
	@echo " - ${ECHO_BOLD}PMSIS_OS=<value>${ECHO_CLEAR} : select the OS (freertos or pulpos)"
	@echo ""
	@echo "For more information, please refer to the SDK documentation."

ifeq '$(PMSIS_OS)' 'pulpos'
ifeq '$(TARGET_CHIP)' 'GAP9_V2'
export USE_PULPOS=1
endif
endif

APP_INC += $(TILER_EMU_INC)

ifndef USE_PULPOS
ifeq ($(BOARD_NAME), gapuino)
COMMON_CFLAGS          += -DCONFIG_GAPUINO

else ifeq ($(BOARD_NAME), gapoc_a)
COMMON_CFLAGS          += -DCONFIG_GAPOC_A
PLPBRIDGE_EXTRA_FLAGS        += -ftdi

else ifeq ($(BOARD_NAME), gapoc_a_revb)
COMMON_CFLAGS          += -DCONFIG_GAPOC_A
PLPBRIDGE_EXTRA_FLAGS        += -ftdi

else ifeq ($(BOARD_NAME), gapoc_b)
COMMON_CFLAGS          += -DCONFIG_GAPOC_B
PLPBRIDGE_EXTRA_FLAGS        += -ftdi

else ifeq ($(BOARD_NAME), gapoc_b_revb)
COMMON_CFLAGS          += -DCONFIG_GAPOC_B
PLPBRIDGE_EXTRA_FLAGS        += -ftdi

else ifeq ($(BOARD_NAME), ai_deck)
COMMON_CFLAGS          += -DCONFIG_AI_DECK

else ifeq ($(BOARD_NAME), vega)
COMMON_CFLAGS          += -DCONFIG_VEGA -mvega -Wa,-mwinsn

else ifeq ($(BOARD_NAME), gap9_v2)
COMMON_CFLAGS          += -DCONFIG_GAP9_V2
endif				# BOARD_NAME
endif

ifdef RUNNER_CONFIG
override config_args += --config-ini=$(RUNNER_CONFIG)
endif

# Enable traces with GVSOC
ifeq ($(platform), gvsoc)
ifeq ($(ENABLE_CORE_TRACES), 1)
override runner_args += "--trace=fc/insn:insn_fc.log"
override runner_args += "--trace=pe0/insn:insn_pe0.log"
override runner_args += "--trace=pe1/insn:insn_pe1.log"
override runner_args += "--trace=pe2/insn:insn_pe2.log"
override runner_args += "--trace=pe3/insn:insn_pe3.log"
override runner_args += "--trace=pe4/insn:insn_pe4.log"
override runner_args += "--trace=pe5/insn:insn_pe5.log"
override runner_args += "--trace=pe6/insn:insn_pe6.log"
override runner_args += "--trace=pe7/insn:insn_pe7.log"
override runner_args += "--trace=pe8/insn:insn_pe8.log"
override runner_args += "--trace=cluster/event_unit:cl_eu.log"
override runner_args += "--trace=cluster/dma:cl_dma.log"
endif				# ENABLE_CORE_TRACES
endif				# platform

ifdef runner_args
export GVSOC_OPTIONS=$(runner_args)
endif


# Configuration GAP9

CONFIG_FAST_OSC_FREQUENCY    ?= 24576063 # Fast clock freq
CONFIG_FLL_MAXDCO_FREQ       ?= 900000000

CONFIG_FREQUENCY_PERIPH      ?= 160000000
CONFIG_FREQUENCY_FC          ?= 50000000
CONFIG_FREQUENCY_CLUSTER     ?= 50000000
CONFIG_FREQUENCY_SFU         ?= 50000000

CONFIG_MAX_FREQUENCY_PERIPH  ?= 0
CONFIG_MAX_FREQUENCY_FC      ?= 0
CONFIG_MAX_FREQUENCY_CLUSTER ?= 0
CONFIG_MAX_FREQUENCY_SFU     ?= 0

CONFIG_FREQUENCY_FPGA        ?= 50000000

override APP_CFLAGS += -DCONFIG_FLL_MAXDCO_FREQ=$(CONFIG_FLL_MAXDCO_FREQ)

ifdef CONFIG_OPEN_LOOP
override APP_CFLAGS += -DCONFIG_OPEN_LOOP
endif				# CONFIG_OPEN_LOOP

ifdef CONFIG_NO_FAST_OSC
override APP_CFLAGS += -DCONFIG_NO_FAST_OSC
endif				# CONFIG_NO_FAST_OSC

ifdef CONFIG_I2S_FAST_CLOCK
override APP_CFLAGS += -DCONFIG_I2S_FAST_CLOCK
endif				# CONFIG_I2S_FAST_CLOCK

ifdef CONFIG_SLOW_OSC
override APP_CFLAGS += -DCONFIG_SLOW_OSC
# In case we need slow ref clock, activate it after wakeup from the rom otherwise it will shut it down
# which will disturbs the RTC
override config_args += --config-opt=**/runner/efuses/content/info_2/wake_osc_ctrl=3
endif				# CONFIG_SLOW_OSC


CONFIG_BOOT_DEVICE           ?= hyperflash

ifeq '$(CONFIG_BOOT_DEVICE)' 'hyperflash'
CONFIG_BOOT_DEVICE_FREQUENCY ?= 166000000
endif

ifeq '$(CONFIG_BOOT_DEVICE)' 'spiflash'
CONFIG_BOOT_DEVICE_FREQUENCY ?= 166000000
override config_args += --config-opt=**/runner/boot/device=target/board/devices/spiflash
endif

ifeq '$(CONFIG_BOOT_DEVICE)' 'mram'
CONFIG_BOOT_DEVICE_FREQUENCY ?= 25000000
override config_args += --config-opt=**/runner/boot/device=target/board/devices/mram
endif

ifdef CONFIG_BOOT_MODE
override config_args += --config-opt=**/runner/boot/mode=$(CONFIG_BOOT_MODE)

ifeq '$(CONFIG_BOOT_MODE)' 'spislave'
# Spi slave boot for now requires the testbench to do the loading
export GVSOC_TESTBENCH=1
override config_args += --config-opt=testbench/testbench/spislave_boot/enabled=true
override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

override APP_CFLAGS           += -DCONFIG_BOOT_MODE_SPISLAVE=1
endif				# CONFIG_BOOT_MODE
endif				# CONFIG_BOOT_MODE

ifdef CONFIG_NO_EFUSE
override config_args += --config-opt=**/runner/efuses/enabled=false
endif

ifdef CONFIG_JTAG_FORCE
override config_args += --config-opt=**/runner/boot/jtag_force=true
endif


ifdef GV_PROXY
override config_args += --config-opt=**/gvsoc/proxy/enabled=true
endif

ifdef GV_PROXY_PORT
override config_args += --config-opt=**/gvsoc/proxy/port=$(GV_PROXY_PORT)
endif

ifdef GV_DEBUG_MODE
override config_args += --config-opt=**/gvsoc/debug-mode=true
endif


ifdef CONFIG_TESTBENCH
# Set of models connected to the RTL platform, capable of generating stimuli.
# For example, on I2S it allows to generate samples, or capture them on I2S interfaces.
# Models are dynamically controlled via UART.
# A library in gap_lib is needed to control them.
export GVSOC_TESTBENCH=1
CONFIG_BOARD_PATH ?= target
override config_args += --config-opt=$(CONFIG_BOARD_PATH)/addon_testbench_enabled=true

CONFIG_TESTBENCH_UART_ID ?= 1
CONFIG_TESTBENCH_UART_BAUDRATE ?= 20000000

APP_SRCS           += $(GAP_LIB_PATH)/testbench/testbench.c $(GAP_LIB_PATH)/testbench/testlib.c
APP_INC            += $(GAP_LIB_PATH)/testbench
override APP_CFLAGS         += -DCONFIG_TESTBENCH_UART_ID=$(CONFIG_TESTBENCH_UART_ID) \
                      -DCONFIG_TESTBENCH_UART_BAUDRATE=$(CONFIG_TESTBENCH_UART_BAUDRATE)

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true
override config_args += --config-opt=**/testbench/testbench/uart_id=$(CONFIG_TESTBENCH_UART_ID)
override config_args += --config-opt=**/testbench/testbench/uart_baudrate=$(CONFIG_TESTBENCH_UART_BAUDRATE)

endif				# CONFIG_TESTBENCH

ifdef CONFIG_IO_UART_BAUDRATE
override config_args += --config-opt=**/rtl/testbench/uart/baudrate=$(CONFIG_IO_UART_BAUDRATE)
endif				# CONFIG_IO_UART_BAUDRATE


# FS config
READFS_FLASH ?= flash

override config_args += $(foreach file, $(READFS_FILES), --config-opt=**/$(READFS_FLASH)/content/partitions/readfs/files=$(file))
override config_args += $(foreach file, $(HOSTFS_FILES), --config-opt=**/flash/content/partitions/hostfs/files=$(file))

ifdef LFS_ROOT_DIR
override config_args += --config-opt=**/flash/content/partitions/lfs/root_dir=$(LFS_ROOT_DIR)
endif				# LFS_ROOT_DIR


# GAP_LIB sources
ifeq '$(CONFIG_GAP_LIB_IMGIO)' '1'
APP_SRCS           += $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_INC            += $(GAP_LIB_PATH)/include $(GAP_LIB_PATH)/include/gaplib
endif				# CONFIG_GAP_LIB_IMGIO

ifeq '$(CONFIG_GAP_LIB_JPEG)' '1'
APP_SRCS           += $(GAP_LIB_PATH)/jpeg/dct.c \
                      $(GAP_LIB_PATH)/jpeg/jpeg_constants.c \
                      $(GAP_LIB_PATH)/jpeg/jpeg_encoder.c \
                      $(GAP_LIB_PATH)/jpeg/cluster.c
APP_INC            += $(GAP_LIB_PATH)/include $(GAP_LIB_PATH)/include/gaplib
endif				# CONFIG_GAP_LIB_JPEG

ifeq '$(CONFIG_GAP_LIB_WAVIO)' '1'
APP_SRCS           += $(GAP_LIB_PATH)/wav_io/wavIO.c
APP_INC            += $(GAP_LIB_PATH)/include $(GAP_LIB_PATH)/include/gaplib
endif				# CONFIG_GAP_LIB_WAVIO


ifdef GAPY_PY_TARGET
ifeq '$(platform)' 'gvsoc'
use_py_target=1
endif
endif

ifeq '$(use_py_target)' '1'
GAPY_TARGET_OPT = --py-target=$(GAPY_PY_TARGET)
else
GAPY_TARGET_OPT = --target=$(GAPY_TARGET)
endif


# Directory containing built objects
PMSIS_OS_UPPERCASE  = $(shell echo $(PMSIS_OS) | tr a-z A-Z)
BUILDDIR            = $(CURDIR)/BUILD/$(TARGET_CHIP)/GCC_RISCV_$(PMSIS_OS_UPPERCASE)$(build_dir_ext)
TARGET_BUILD_DIR    = $(BUILDDIR)
#$(info ## BUILDDIR : $(BUILDDIR))

#$(info ## App sources : $(APP_SRCS))
#$(info ## App includes : $(APP_INC))
#$(info ## App cflags : $(APP_CFLAGS))
#$(info ## App ldflags : $(APP_LDFLAGS))

ifeq '$(PMSIS_OS)' 'freertos'

# Select PMSIS drivers
PMSIS_BSP_DIR            = $(GAP_SDK_HOME)/rtos/pmsis/pmsis_bsp

ifeq ($(CUSTOM_BSP),)
include $(PMSIS_BSP_DIR)/rules/freertos_bsp_rules.mk
endif				# CUSTOM_BSP

# Special flag for FreeRTOS to use semihosting.
ifeq ($(FS_TYPE), host)
io = host
endif				# FS_TYPE

APP_SRC                  = $(APP_SRCS)
APP_ASM_SRC              = $(APP_ASM_SRCS)
APP_INC_PATH             = $(APP_INC)
# Compiler options in $(APP_CFLAGS) are passed to compiler in subMakefile.
# Linker options in $(APP_LDFLAGS) are passed to linker in subMakefile.
# Custom linker script.
LINK_SCRIPT              = $(APP_LINK_SCRIPT)
FREERTOS_FLAGS          += $(COMMON_CFLAGS) -D__USE_TCDM_MALLOC__=1 -DPMSIS_DRIVERS=1
FREERTOS_FLAGS          += -D__FC_MALLOC_NATIVE__=0 -D__L2_MALLOC_NATIVE__=0 \
                           -D__PMSIS_L2_MALLOC_NATIVE__=0

#$(info ## FreeRTOS flags : $(FREERTOS_FLAGS))

include $(GAP_SDK_HOME)/rtos/freeRTOS/vendors/gwt/rules/freeRTOS_rules.mk

else

ifeq '$(PMSIS_OS)' 'zephyr'

include $(GAP_SDK_HOME)/utils/rules/zephyr_rules.mk

else

ifeq '$(USE_PULPOS)' '1'
ifndef PULP_APP
PULP_APP = $(APP)
endif
PULP_APP_SRCS     += $(APP_SRCS)
PULP_APP_ASM_SRCS += $(APP_ASM_SRCS)
PULP_CFLAGS       += $(foreach d, $(APP_INC), -I$d) $(APP_CFLAGS) $(COMMON_CFLAGS)
PULP_LDFLAGS      += $(APP_LDFLAGS) $(COMMON_LDFLAGS)

include $(PULPOS_HOME)/rules/pulpos.mk

BIN           = $(TARGETS)

else

USE_OLD_PULPOS = 1

PULP_APP = $(APP)
PULP_APP_FC_SRCS   = $(APP_SRCS)
PULP_APP_ASM_SRCS += $(APP_ASM_SRCS)
PULP_APP_INC_PATH += $(APP_INC)

PULP_CFLAGS = $(APP_CFLAGS) $(COMMON_CFLAGS)
LIBS += $(APP_LDFLAGS) $(COMMON_LDFLAGS)
ifdef USE_PMSIS_TOOLS
PULP_LDFLAGS += -lpitools
endif

ifeq ($(CONFIG_GAP_LIB), 1)
LIBS += -lgaplib
endif				# CONFIG_GAP_LIB

include $(GAP_SDK_HOME)/utils/rules/pulp_rules.mk

endif

endif

endif

#
# GAPY
#

ifeq '$(VERBOSE)' '1'
override gapy_args += -v
endif

include_fs_targets = 1


ifeq '$(platform)' 'gvsoc'

ifeq '$(USE_PULPOS)' '1'
include_fs_targets = 0
endif

ifeq '$(USE_OLD_PULPOS)' '1'
include_fs_targets = 0
endif

ifeq ($(PMSIS_OS), freertos)
include_fs_targets = 0
endif

endif



ifeq '$(include_fs_targets)' '1'

#
# ReadFS
#

# run_gapuino legacy: convert BRIDGE files to readfs makefile input
READFS_FILES ?= $(filter-out -%, $(PLPBRIDGE_FLAGS))

#include $(GAP_SDK_HOME)/tools/gapy/rules/readfs.mk

#
# Littlefs
#

include $(GAP_SDK_HOME)/utils/gapy/rules/littlefs.mk

#
# Flash tools
#
include $(GAP_SDK_HOME)/utils/gapy/rules/flash.mk

endif

PROFILER_SIGNAL_TREE ?= $(GAP_SDK_HOME)/tools/profiler/gui/images/signalstree_GAP8.txt

vscode:
	mkdir -p .vscode
	mkdir -p .vscode/scripts
	cp -r $(GAP_SDK_HOME)/utils/rules/vscode/settings.json .vscode
	cat $(GAP_SDK_HOME)/utils/rules/vscode/tasks.json | sed s#@GAP_TOOLCHAIN@#$(GAP_RISCV_GCC_TOOLCHAIN)#g | sed s#@GAP_SDK@#$(GAP_SDK_HOME)#g > .vscode/tasks.json
	cat $(GAP_SDK_HOME)/utils/rules/vscode/launch.json | sed s#@BIN@#$(BIN)#g | sed s#@GDB@#`which riscv64-unknown-elf-gdb`# > .vscode/launch.json

profiler:
	gapy $(GAPY_TARGET_OPT) --platform=$(platform) --work-dir=$(BUILDDIR) $(config_args) $(gapy_args) --config-opt="gvsoc/debug-mode=true" --config-opt="gvsoc/events/gen_gtkw=false" run --image --flash --exec-prepare --binary=$(BIN) --event-format=raw --event=.*@all.bin $(runner_args)
	cd $(BUILDDIR) && if [ -e all.bin ]; then rm all.bin; fi; mkfifo all.bin
	cd $(BUILDDIR) && export PULP_CONFIG_FILE=$(BUILDDIR)/gvsoc_config.json && profiler $(BUILDDIR) $(BIN) gvsoc_config.json --signal-tree-file=$(PROFILER_SIGNAL_TREE)

size:
	@$(GAP_SDK_HOME)/utils/bin/binary-size --binary=$(BIN) --depth=10 --groups=$(PMSIS_OS) >> $(BIN).size
