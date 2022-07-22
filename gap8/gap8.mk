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
	@echo ""
	@echo "Note: to speed up compilation, you can use the \"-j\" option with most rules"

gap8.all: minimal gvsoc autotiler nntool littlefs.all openocd.all

clean: littlefs.clean
	$(RM) $(TARGET_INSTALL_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) $(BUILD_DIR)
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v3/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v3 clean; fi
	if [ -e tools/profiler ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/profiler clean; fi

minimal: pulp-os freertos gapy.all openocd_tools.all plptest.all

# Compat
sdk: all
mini_checkout: examples.checkout
freertos: freertos.all mini_checkout


$(INSTALL_BIN_DIR) $(TARGET_INSTALL_DIR) $(INSTALL_DIR):
	$(MKDIR) -p $@

install_others: | $(INSTALL_BIN_DIR)
	$(CP) $(GAP_SDK_HOME)/utils/bin/* $(INSTALL_DIR)/bin
	$(CP) $(GAP_SDK_HOME)/tools/ld $(INSTALL_DIR)
	$(CP) $(GAP_SDK_HOME)/utils/rules $(INSTALL_DIR)


# This rules is to compile pmsis-bsp for pulp os on gap8, wihtout having to recompile the whole sdk
pmsis-bsp.build:
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/bsp all

gap_configs.checkout:
	git submodule update --init utils/gap_configs

# PulpOS_v1
pulprt.all: pulprt.checkout pulprt.build

pulprt.checkout:
	cd gap8 && git submodule update --init rtos/pulp/pulp-os

pulprt.build:
	$(MAKE) -C $(GAP_SDK_HOME)/gap8/rtos/pulp build.pulprt

pulp-os: $(TARGET_INSTALL_DIR) install_others pulprt.checkout
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/api -f tools/export.mk build
	$(MAKE) -C $(GAP_SDK_HOME)/gap8/rtos/pulp build
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/bsp all
	$(MAKE) -C $(GAP_SDK_HOME)/libs/gap_lib all

# PulpOS_v2
pulpos.all: pulpos.checkout

gap_lib.checkout:
	cd $(GAP_SDK_HOME) && git submodule update --init libs/gap_lib

pulpos.checkout: gap_lib.checkout
	cd gap8 && git submodule update --init rtos/pulp/hal_pulp rtos/pulp/archi_pulp

freertos.all: freertos.checkout

freertos.checkout:
	git submodule update --recursive --init rtos/freeRTOS

use_old_gvsoc=0

ifeq '$(TARGET_CHIP_FAMILY)' 'GAP8'
use_old_gvsoc=1
endif

ifeq '$(TARGET_CHIP)' 'GAP9'
use_old_gvsoc=1
endif

# GVSOC
ifeq '$(use_old_gvsoc)' '1'
gvsoc: pulp-os pulpos.all gvsoc.checkout
	cd $(GAP_SDK_HOME) && ./gvsoc/gvsoc-build
	$(MAKE) -f gap8/gap8.mk gvsoc.build
else
gvsoc: pulp-os
	$(MAKE) -f gap8/gap8.mk gvsoc.build
endif

gvsoc.all: gvsoc.checkout gvsoc.build

gvsoc.checkout:
	git submodule update --init --recursive gvsoc

ifeq '$(use_old_gvsoc)' '1'
gvsoc.build: gvsoc.checkout gap_configs.checkout
	cd $(GAP_SDK_HOME) && $(MAKE) -C gvsoc/gvsoc build BUILD_DIR=$(BUILD_DIR)/gvsoc INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/install
	cd $(GAP_SDK_HOME) && $(MAKE) -C gvsoc/gvsoc_gap build BUILD_DIR=$(BUILD_DIR)/gvsoc_gap INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/install
else
gvsoc.build:
	cmake -S gvsoc -B build/gvsoc -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTARGET_CHIP=GAP9_V2 && cmake --build build/gvsoc -j8 && cmake --install build/gvsoc --prefix=$(GAP_SDK_HOME)/install/workstation
endif

gvsoc.clean:
	cd $(GAP_SDK_HOME) && $(MAKE) -C gvsoc/gvsoc clean BUILD_DIR=$(BUILD_DIR)/gvsoc INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/install


# Tools

openocd.all: openocd.build

openocd.checkout:
	if [ ! -e utils/gap8-openocd ]; then \
		git clone --recursive https://github.com/GreenWaves-Technologies/gap8_openocd.git utils/gap8-openocd; \
		cd utils/gap8-openocd; \
	fi

openocd.build: openocd.checkout
	cd utils/gap8-openocd && ./bootstrap
	cd utils/gap8-openocd && ./configure --prefix=$(INSTALL_DIR)/gap8-openocd --program-prefix=gap8-
	cd utils/gap8-openocd && make -j install
	mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd utils/openocd_tools && cp -r tcl/* $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd utils/openocd_tools && mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins && cp -r gap_bins/* $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins

openocd_tools.all: openocd_tools.build

openocd_tools.build:
	mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd utils/openocd_tools && cp -r tcl/* $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd utils/openocd_tools && mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins && cp -r gap_bins/* $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins

profiler:
	$(MAKE) -C tools/profiler all
	mkdir -p $(INSTALL_DIR)/bin
	cp tools/profiler/gui/build/profiler $(INSTALL_DIR)/bin

nntool:
	$(MAKE) -C $(GAP_SDK_HOME)/tools/nntool/nntool req

sfu.clean:
ifneq ("$(wildcard tools/sfu_gen/Makefile)","")
	cd tools/sfu_gen && make clean
endif

sfu.build:
ifneq ("$(wildcard tools/sfu_gen/Makefile)","")
	cd tools/sfu_gen && make lib all install #INSTALL_DIR=$(CURDIR)/tools/sfu_gen/install
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

littlefs.build: $(LFS_MAKEFILE)
	make -C $(LFS_BUILD_DIR) all install

$(LFS_MAKEFILE): $(LFS_DIR)/CMakeLists.txt | $(LFS_BUILD_DIR)
	cd $(LFS_BUILD_DIR) && cmake -DCMAKE_INSTALL_PREFIX:PATH=$(INSTALL_DIR) $(LFS_DIR)

$(LFS_BUILD_DIR):
	$(MKDIR) -p $@

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


.PHONY: all clean install_others pulp-os gvsoc minimal
