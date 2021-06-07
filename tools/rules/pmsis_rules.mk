BOARD_NAME ?= gapuino

ifeq ($(TARGET_CHIP_FAMILY), GAP9)
PMSIS_OS ?= freertos
else
PMSIS_OS ?=pulpos
endif

ifndef platform
ifdef PMSIS_PLATFORM
platform = $(PMSIS_PLATFORM)
endif
endif

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
COMMON_CFLAGS          += -DCONFIG_VEGA

else ifeq ($(BOARD_NAME), gap9_v2)
COMMON_CFLAGS          += -DCONFIG_GAP9_V2
endif				# BOARD_NAME
endif

ifdef RUNNER_CONFIG
override config_args += --config-ini=$(RUNNER_CONFIG)
endif

ifdef runner_args
export GVSOC_OPTIONS=$(runner_args)
endif


# Configuration

CONFIG_BOOT_DEVICE ?= hyperflash
CONFIG_FREQUENCY_PERIPH ?= 160000000
CONFIG_FREQUENCY_FC ?= 50000000
CONFIG_FREQUENCY_CLUSTER ?= 50000000
CONFIG_FREQUENCY_SFU ?= 50000000
CONFIG_FREQUENCY_FPGA ?= 50000000

ifeq '$(CONFIG_BOOT_DEVICE)' 'hyperflash'
CONFIG_BOOT_DEVICE_FREQUENCY ?= 166000000
endif

ifeq '$(CONFIG_BOOT_DEVICE)' 'spiflash'
CONFIG_BOOT_DEVICE_FREQUENCY ?= 166000000
override config_args += --config-opt=runner/boot/device=target/board/devices/spiflash
endif

ifeq '$(CONFIG_BOOT_DEVICE)' 'mram'
CONFIG_BOOT_DEVICE_FREQUENCY ?= 25000000
override config_args += --config-opt=runner/boot/device=target/board/devices/mram
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


# GAP_LIB sources
ifeq '$(CONFIG_GAP_LIB_IMGIO)' '1'
GAP_LIB_PATH        = $(GAP_SDK_HOME)/libs/gap_lib
APP_SRCS           += $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_INC            += $(GAP_LIB_PATH)/include $(GAP_LIB_PATH)/include/gaplib
endif				# CONFIG_GAP_LIB_IMGIO

ifeq '$(CONFIG_GAP_LIB_JPEG)' '1'
GAP_LIB_PATH        = $(GAP_SDK_HOME)/libs/gap_lib
APP_SRCS           += $(GAP_LIB_PATH)/jpeg/dct.c \
                      $(GAP_LIB_PATH)/jpeg/jpeg_constants.c \
                      $(GAP_LIB_PATH)/jpeg/jpeg_encoder.c \
                      $(GAP_LIB_PATH)/jpeg/cluster.c
APP_INC            += $(GAP_LIB_PATH)/include $(GAP_LIB_PATH)/include/gaplib
endif				# CONFIG_GAP_LIB_JPEG

ifeq '$(CONFIG_GAP_LIB_WAVIO)' '1'
GAP_LIB_PATH        = $(GAP_SDK_HOME)/libs/gap_lib
APP_SRCS           += $(GAP_LIB_PATH)/wav_io/wavIO.c
APP_INC            += $(GAP_LIB_PATH)/include $(GAP_LIB_PATH)/include/gaplib
endif				# CONFIG_GAP_LIB_WAVIO



ifeq '$(PMSIS_OS)' 'freertos'

# Select PMSIS drivers
ifeq ($(TARGET_CHIP), GAP9)
#NO_PMSIS_BSP             = true
endif				# TARGET_CHIP

ifndef NO_PMSIS_BSP
ifeq ($(GAP_SDK_HOME), )
PMSIS_BSP_DIR            = $(FREERTOS_PATH)/vendors/gwt/TARGET_GWT/pmsis/bsp
else
PMSIS_BSP_DIR            = $(GAP_SDK_HOME)/rtos/pmsis/pmsis_bsp
endif				# GAP_SDK_HOME


ifeq ($(CUSTOM_BSP),)
include $(PMSIS_BSP_DIR)/rules/freertos_bsp_rules.mk
endif

include $(GAP_SDK_HOME)/rtos/openmp/rules/freertos_openmp_rules.mk

# Special flag for FreeRTOS to use semihosting.
ifeq ($(FS_TYPE), host)
io = host
endif				# FS_TYPE

endif				# NO_PMSIS_BSP

APP_SRC                  = $(APP_SRCS)
APP_INC_PATH             = $(APP_INC)
# Compiler options in $(APP_CFLAGS) are passed to compiler in subMakefile.
# Linker options in $(APP_LDFLAGS) are passed to linker in subMakefile.
# Custom linker script.
LINK_SCRIPT              = $(APP_LINK_SCRIPT)
FREERTOS_FLAGS          += $(COMMON_CFLAGS) -D__USE_TCDM_MALLOC__=1 -DPMSIS_DRIVERS=1
FREERTOS_FLAGS          += -D__FC_MALLOC_NATIVE__=0 -D__L2_MALLOC_NATIVE__=0 \
                           -D__PMSIS_L2_MALLOC_NATIVE__=0

#$(info ## FreeRTOS app sources : $(APP_SRC))
#$(info ## FreeRTOS app includes : $(APP_INC_PATH))
#$(info ## FreeRTOS flags : $(FREERTOS_FLAGS))
#$(info ## FreeRTOS libsflags : $(APP_LIBSFLAGS))

include $(GAP_SDK_HOME)/rtos/freeRTOS/vendors/gwt/rules/freeRTOS_rules.mk

else

ifeq '$(PMSIS_OS)' 'mbed'

TEST_C          = $(SRCS)
MBED_FLAGS     += -DMBED_CONF_RTOS_PRESENT=1

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

else

ifeq '$(PMSIS_OS)' 'zephyr'

include $(GAP_SDK_HOME)/tools/rules/zephyr_rules.mk

else

ifeq '$(USE_PULPOS)' '1'
ifndef PULP_APP
PULP_APP = $(APP)
endif
PULP_APP_SRCS += $(APP_SRCS)
PULP_CFLAGS  += $(foreach d, $(APP_INC), -I$d) $(APP_CFLAGS) $(COMMON_CFLAGS)
PULP_LDFLAGS += $(APP_LDFLAGS) $(COMMON_LDFLAGS)

BUILDDIR      = $(CURDIR)/BUILD/$(TARGET_CHIP)/GCC_RISCV$(build_dir_ext)
TARGET_BUILD_DIR = $(BUILDDIR)

include $(PULPOS_HOME)/rules/pulpos.mk

BIN           = $(TARGETS)

else

USE_OLD_PULPOS = 1

PULP_APP = $(APP)
PULP_APP_FC_SRCS = $(APP_SRCS)
PULP_APP_INC_PATH    += $(APP_INC)

PULP_CFLAGS = $(APP_CFLAGS) $(COMMON_CFLAGS)
LIBS += $(APP_LDFLAGS) $(COMMON_LDFLAGS)
ifdef USE_PMSIS_TOOLS
PULP_LDFLAGS += -lpitools
endif

ifeq ($(CONFIG_GAP_LIB), 1)
LIBS += -lgaplib
endif				# CONFIG_GAP_LIB

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk

endif

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

include $(GAP_SDK_HOME)/tools/gapy/rules/littlefs.mk

#
# Flash tools
#
include $(GAP_SDK_HOME)/tools/gapy/rules/flash.mk

endif

vscode:
	mkdir -p .vscode
	mkdir -p .vscode/scripts
	cp -r $(GAP_SDK_HOME)/tools/rules/vscode/settings.json .vscode
	cat $(GAP_SDK_HOME)/tools/rules/vscode/tasks.json | sed s#@GAP_TOOLCHAIN@#$(GAP_RISCV_GCC_TOOLCHAIN)#g | sed s#@GAP_SDK@#$(GAP_SDK_HOME)#g > .vscode/tasks.json
	cat $(GAP_SDK_HOME)/tools/rules/vscode/launch.json | sed s#@BIN@#$(BIN)#g | sed s#@GDB@#`which riscv64-unknown-elf-gdb`# > .vscode/launch.json
