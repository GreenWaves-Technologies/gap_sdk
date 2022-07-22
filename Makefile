# Copyright (c) 2021 GreenWaves Technologies SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of GreenWaves Technologies SAS nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

ifeq ($(TARGET_CHIP), GAP9)
use_old_build = 1
endif

ifeq ($(TARGET_CHIP_FAMILY), GAP8)
use_old_build = 1
endif

ifeq '$(use_old_build)' '1'

checkout:
	git submodule update --init --recursive

all:
	$(MAKE) -f gap8/gap8.mk gap8.all

sdk: all

audio-framework: all
minimal: all
plptest.all: all
autotiler: all
nntool: all
littlefs.build: all
gvsoc: all
openocd.all: all

clean:
	$(MAKE) -f gap8/gap8.mk clean

else

ifndef GAP_SDK_HOME
    $(error Please run 'source sourceme.sh' in gap_sdk first)
endif				# GAP_SDK_HOME

CD = cd
CP = cp -rf
RM = rm -rf
MKDIR = mkdir -p
MAKE = make

BUILD_DIR ?= $(CURDIR)/build
export BUILD_DIR

INSTALL_BIN_DIR = $(INSTALL_DIR)/bin

TIMEOUT ?= 300

SHELL=bash
ECHO_GREEN = $(shell tput setaf 2)
ECHO_BOLD = $(shell tput bold)
ECHO_CLEAR = $(shell tput sgr0)

# Keep this rule first
help:
	@echo "=================== ${ECHO_BOLD}${ECHO_GREEN}GAP SDK${ECHO_CLEAR} ==================="
	@echo ""
	@echo "Main targets:"
	@echo " - ${ECHO_BOLD}clean${ECHO_CLEAR}       : clean the SDK"
	@echo " - ${ECHO_BOLD}all${ECHO_CLEAR}         : build the whole SDK with all tools"
	@echo " - ${ECHO_BOLD}minimal${ECHO_CLEAR}     : get latest sources for all rtos and libs"
	@echo " - ${ECHO_BOLD}gvsoc${ECHO_CLEAR}       : build GVSOC simulation platform"
	@echo " - ${ECHO_BOLD}openocd.all${ECHO_CLEAR} : build OpenOCD tools to run simulation on boards"
	@echo " - ${ECHO_BOLD}nntool${ECHO_CLEAR}      : build nntool"
	@echo ""
	@echo "Note: to speed up compilation, you can use the \"-j\" option with most rules"

checkout:
	git submodule update --recursive --init

all: minimal gvsoc autotiler nntool littlefs.build

clean: littlefs.clean
	$(RM) $(TARGET_INSTALL_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) $(BUILD_DIR)
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v3/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v3 clean; fi
	if [ -e tools/profiler ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/profiler clean; fi

all: sfu.build
minimal: plptest.build
clean: sfu.clean

# Compat
sdk: all
mini_checkout: pmsis.checkout examples.checkout
freertos: freertos.all mini_checkout


$(INSTALL_BIN_DIR) $(TARGET_INSTALL_DIR) $(INSTALL_DIR):
	$(MKDIR) -p $@

install_others: | $(INSTALL_BIN_DIR)
	$(CP) $(GAP_SDK_HOME)/utils/bin/* $(INSTALL_DIR)/bin
	$(CP) $(GAP_SDK_HOME)/tools/ld $(INSTALL_DIR)
	$(CP) $(GAP_SDK_HOME)/utils/rules $(INSTALL_DIR)


# Sources
pmsis.checkout:
	git submodule update --init rtos/pmsis

gap_configs.checkout:
	git submodule update --init utils/gap_configs

# PulpOS_v2
pulpos.all: pulpos.checkout

gap_lib.checkout:
	git submodule update --init libs/gap_lib

pulpos.checkout: gap_lib.checkout
	git submodule update --init rtos/pulp/pulpos-2 rtos/pulp/pulpos-2_gap9 rtos/pulp/gap_archi

freertos.all: freertos.checkout

freertos.checkout:
	git submodule update --recursive --init rtos/freeRTOS

# GVSOC
gvsoc:
	$(MAKE) gvsoc.build

gvsoc.all: gvsoc.checkout gvsoc.build

gvsoc.checkout:
	git submodule update --init --recursive gvsoc

gvsoc.build:
	# With openmp
	cmake -S gvsoc -B build/gvsoc/$(TARGET_CHIP) -DCMAKE_CXX_FLAGS=-fopenmp -DCMAKE_EXE_LINKER_FLAGS=-fopenmp -DCMAKE_SHARED_LINKER_FLAGS=-fopenmp -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTARGET_CHIP=GAP9_V2 -DCONFIG_GVSOC_SKIP_UDMA_BUILD=$(CONFIG_GVSOC_SKIP_UDMA_BUILD)
	#cmake -S gvsoc -B build/gvsoc/$(TARGET_CHIP) -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTARGET_CHIP=$(TARGET_CHIP) -DCONFIG_GVSOC_SKIP_UDMA_BUILD=$(CONFIG_GVSOC_SKIP_UDMA_BUILD)
	cmake --build build/gvsoc/$(TARGET_CHIP) -j 4
	cmake --install build/gvsoc/$(TARGET_CHIP) --prefix $(GAP_SDK_HOME)/install/workstation

gvsoc.clean:
	$(MAKE) -C gvsoc/gvsoc clean BUILD_DIR=$(BUILD_DIR)/gvsoc INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/install


# Tools

openocd.all: openocd.build

openocd.checkout:
	if [ ! -e utils/openocd ]; then \
		git clone https://github.com/riscv/riscv-openocd.git utils/openocd; \
		cd utils/openocd; \
		git checkout 1449af5bd; \
	fi

openocd.build: openocd.checkout
	cd utils/openocd && ./bootstrap && ./configure --enable-jtag_dpi --disable-werror --prefix=$(INSTALL_DIR)/openocd && $(MAKE) && $(MAKE) install

openocd.clean:
	rm -rf $(INSTALL_DIR)/openocd tools/openocd

PROFILER_V2_DIR = $(GAP_SDK_HOME)/tools/profiler_v2
PROFILER_V2_BUILD_DIR = $(GAP_SDK_HOME)/build/profiler_v2

profiler_v2:
	cmake -S $(PROFILER_V2_DIR) -B $(PROFILER_V2_BUILD_DIR)
	cmake --build $(PROFILER_V2_BUILD_DIR)
	cmake --install $(PROFILER_V2_BUILD_DIR) --prefix $(INSTALL_DIR)

nntool:
	$(MAKE) -C $(GAP_SDK_HOME)/tools/nntool/nntool req


# SFUConfigurationGenerator (aka sfu_gen)
SFUGEN_DIR = $(GAP_SDK_HOME)/tools/sfu_gen
SFUGEN_BUILD_DIR = $(GAP_SDK_HOME)/build/sfu_gen

sfu.clean:
ifneq ("$(wildcard tools/sfu_gen/Makefile)","")
	rm -rf $(SFUGEN_BUILD_DIR)
endif

sfu.build:
ifneq ("$(wildcard tools/sfu_gen/CMakeLists.txt)","")
	cmake -S $(SFUGEN_DIR) -B $(SFUGEN_BUILD_DIR)
	cmake --build $(SFUGEN_BUILD_DIR)
	cmake --install $(SFUGEN_BUILD_DIR) --prefix $(INSTALL_DIR)
endif

# Utils
gapy.all: gapy.checkout

gapy.checkout:
	git submodule update --init utils/gapy

plptest.all: plptest.checkout plptest.build

plptest.checkout:
	git submodule update --init utils/plptest

plptest.build:
	if [ -e utils/plptest ]; then cd utils/plptest && make build; fi


# Littlefs
LFS_DIR = $(GAP_SDK_HOME)/utils/littlefs
LFS_BUILD_DIR = $(LFS_DIR)/build
LFS_MAKEFILE = $(LFS_BUILD_DIR)/Makefile

#include $(LFS_MAKEFILE)

littlefs.build:
	cmake -S $(LFS_DIR) -B build/lfs
	cmake --build build/lfs
	cmake --install build/lfs --prefix $(INSTALL_DIR)

littlefs.checkout:
	git submodule update --init utils/littlefs

littlefs.all: littlefs.checkout littlefs.build

littlefs.clean:
	rm -rf $(LFS_BUILD_DIR)



# Libs
autotiler:
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v3/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v3 all; fi

audio-framework.checkout:
	git submodule update --init tools/audio-framework

audio-framework.build:
	mkdir -p $(BUILD_DIR)/audio-framework
	cd $(BUILD_DIR)/audio-framework && cmake $(GAP_SDK_HOME)/tools/audio-framework \
		-DCMAKE_INSTALL_PREFIX=$(GAP_SDK_HOME)/install/workstation \
		-DAF_COMPONENTS_PATH=$(GAP_SDK_HOME)/tools/audio-framework/components \
		-DAF_LIBRARY_PATH=$(GAP_SDK_HOME)/install/workstation/lib
	cd $(BUILD_DIR)/audio-framework && make all install

audio-framework.clean:
	rm -rf $(BUILD_DIR)/audio-framework

audio-framework: audio-framework.checkout audio-framework.build

# Examples, tests
examples.checkout:
	git submodule update --recursive --init examples

tests.checkout:
	git submodule update --init tests/sfu_tests

test:
	plptest --max-timeout=$(TIMEOUT) --bench-csv-file=results.csv
	plptest --max-timeout=$(TIMEOUT) --bench-csv-file=results.csv score


.PHONY: all clean install_others gvsoc minimal

endif
