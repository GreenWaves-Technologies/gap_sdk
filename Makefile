# Copyright (c) 2017 GreenWaves Technologies SAS
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
endif

BUILD_DIR ?= $(CURDIR)/build
export BUILD_DIR

CD = cd
CP = cp -rf
RM = rm -rf
MKDIR = mkdir -p
MAKE = make

TIMEOUT ?= 300

PULP_BRIDGE_PATH = $(GAP_SDK_HOME)/tools/pulp_tools/pulp-debug-bridge

checkout:
	git submodule update --recursive --init

ifeq ($(TARGET_CHIP_FAMILY), GAP8)
sdk: all autotiler nntool openocd.build
all: pulp-os tools gvsoc flasher docs littlefs.build openocd_tools.build

clean: littlefs.clean
	$(RM) $(TARGET_INSTALL_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) $(BUILD_DIR)
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v2/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v2 clean; fi
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v3/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v3 clean; fi
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools clean
	$(MAKE) -C $(GAP_SDK_HOME)/tools/nntool clean
	$(MAKE) -C $(GAP_SDK_HOME)/docs clean
	if [ -e tools/profiler ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/profiler clean; fi

else
sdk: all autotiler nntool
all: pulp-os gvsoc littlefs.build

clean: littlefs.clean
	$(RM) $(TARGET_INSTALL_DIR)
	$(RM) $(BUILD_DIR)
endif
minimal_sdk: freertos pmsis-bsp.checkout pmsis-api.checkout gapy.all examples.checkout openocd_tools.build
freertos: freertos.all openmp.all gap_lib.all

# Rules for installing docs
#------------------------------------------
docs:
	$(MAKE) -C $(GAP_SDK_HOME)/docs all

# Rules for installing tools
#------------------------------------------
INSTALL_BIN_DIR = $(INSTALL_DIR)/bin

$(INSTALL_BIN_DIR) $(TARGET_INSTALL_DIR) $(INSTALL_DIR):
	$(MKDIR) -p $@

install_others: | $(INSTALL_BIN_DIR)
	$(CP) $(GAP_SDK_HOME)/tools/bin/* $(INSTALL_DIR)/bin
	$(CP) $(GAP_SDK_HOME)/tools/ld $(INSTALL_DIR)
	$(CP) $(GAP_SDK_HOME)/tools/rules $(INSTALL_DIR)

install_pulp_tools: install_others plptest.build
	#$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools all

tools: install_others install_pulp_tools

nntool:
	$(MAKE) -C $(GAP_SDK_HOME)/tools/nntool all

pulp-os: install_pulp_tools | $(TARGET_INSTALL_DIR)
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/pmsis_api -f tools/export.mk build
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pulp build
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/pmsis_bsp all
	$(MAKE) -C $(GAP_SDK_HOME)/libs/gap_lib all

gap_tools:
	$(MAKE) -C $(GAP_SDK_HOME)/tools/gap_tools all

flasher: pulp-os
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools/gap_flasher install

gvsoc: pulp-os tools
	./gvsoc/gvsoc-build

autotiler:
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v2/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v2 all; fi
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v3/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v3 all; fi

gap_lib.checkout:
	git submodule update --init --recursive libs/gap_lib

gap_lib.all: gap_lib.checkout


ifeq ($(TARGET_CHIP_FAMILY), GAP8)

openocd_tools.checkout:
	git submodule update --init --recursive tools/gap8-openocd-tools

openocd_tools.build:
	mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd tools/gap8-openocd-tools && cp -r tcl/* $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd tools/gap8-openocd-tools && mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins && cp -r gap_bins/* $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins

openocd_tools.all: openocd_tools.checkout openocd_tools.build


openocd.checkout:
	git submodule update --init --recursive tools/gap8-openocd

openocd.build:
	cd tools/gap8-openocd && ./bootstrap
	cd tools/gap8-openocd && ./configure --prefix=$(INSTALL_DIR)/gap8-openocd --program-prefix=gap8-
	cd tools/gap8-openocd && make -j install
	mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd tools/gap8-openocd-tools && cp -r tcl/* $(INSTALL_DIR)/gap8-openocd/share/openocd/scripts/tcl
	cd tools/gap8-openocd-tools && mkdir -p $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins && cp -r gap_bins/* $(INSTALL_DIR)/gap8-openocd/share/openocd/gap_bins

openocd.all: openocd.checkout openocd.build

else

openocd.clean:
	rm -rf $(INSTALL_DIR)/openocd tools/openocd

openocd.checkout:
	if [ ! -e tools/openocd ]; then \
		git clone https://github.com/riscv/riscv-openocd.git tools/openocd; \
		cd tools/openocd; \
		git checkout c56aa667c2ffee906a6d7a7084b70bece863fc73; \
	fi

openocd.build:
	cd tools/openocd && ./bootstrap && ./configure --enable-jtag_dpi --prefix=$(INSTALL_DIR)/openocd && make && make install

openocd.all: openocd.checkout openocd.build

endif


profiler:
	$(MAKE) -C tools/profiler all
	mkdir -p $(INSTALL_DIR)/bin
	cp tools/profiler/gui/build/profiler $(INSTALL_DIR)/bin


test:
	plptest --max-timeout=$(TIMEOUT) --bench-csv-file=results.csv
	plptest --max-timeout=$(TIMEOUT) --bench-csv-file=results.csv score

#
# Littlefs
#
LFS_DIR = $(GAP_SDK_HOME)/tools/littlefs
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
	git submodule update --init tools/littlefs

littlefs.all: littlefs.checkout littlefs.build

littlefs.clean:
	rm -rf $(LFS_BUILD_DIR)


plptest.checkout:
	git submodule update --init tools/plptest

plptest.build:
	if [ -e tools/plptest ]; then cd tools/plptest && make build; fi

plptest.all: plptest.checkout plptest.build



gap-configs.checkout:
	git submodule update --init tools/gap-configs

gap-configs.all: gap-configs.checkout



gapy.checkout: gap-configs.checkout
	git submodule update --init tools/gapy

gapy.all: gapy.checkout gap-configs.all



tests.checkout:
	git submodule update --init tests/pmsis_tests tests/bsp_tests tests/sfu_tests


pulpos.checkout:
	git submodule update --init rtos/pulp/pulpos-2 rtos/pulp/pulpos-2_gap8 rtos/pulp/pulpos-2_gap9 rtos/pulp/gap_archi rtos/pmsis/pmsis_api rtos/pmsis/pmsis_bsp

pulpos.all: pulpos.checkout


pmsis-bsp.checkout:
	git submodule update --init rtos/pmsis/pmsis_bsp

pmsis-bsp.build:
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/pmsis_bsp all

pmsis-bsp.all: pmsis-bsp.checkout pmsis-bsp.build

pmsis-api.checkout:
	git submodule update --init rtos/pmsis/pmsis_api

pulprt.checkout:
	git submodule update --init rtos/pulp/pulp-os

pulprt.build:
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pulp build.pulprt

pulprt.all: pulprt.checkout pulprt.build


freertos.checkout:
	git submodule update --recursive --init rtos/freeRTOS

freertos.all: freertos.checkout


gvsoc.checkout:
	git submodule update --init gvsoc/gvsoc gvsoc/gvsoc_gap gvsoc/gvsoc_gap_sfu

gvsoc.build:
	make -C gvsoc/gvsoc build BUILD_DIR=$(BUILD_DIR)/gvsoc INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/install
	make -C gvsoc/gvsoc_gap build BUILD_DIR=$(BUILD_DIR)/gvsoc_gap INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/install

gvsoc.clean:
	make -C gvsoc/gvsoc clean BUILD_DIR=$(BUILD_DIR)/gvsoc INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/install

gvsoc.all: gvsoc.checkout gvsoc.build


openmp.checkout:
	git submodule update --recursive --init rtos/openmp

openmp.all: openmp.checkout

examples.checkout:
	git submodule update --recursive --init examples




.PHONY: all install clean docs install_others install_pulp_tools tools pulp-os gvsoc flasher
