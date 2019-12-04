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

PULP_BRIDGE_PATH = $(GAP_SDK_HOME)/tools/pulp_tools/pulp-debug-bridge

ifeq ($(TARGET_CHIP), GAP8)
sdk:: all autotiler nntool openocd
all:: pulp-os tools gvsoc flasher docs
clean:
	$(RM) $(TARGET_INSTALL_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) $(BUILD_DIR)
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v2/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v2 clean; fi
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v3/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v3 clean; fi
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools clean
	$(MAKE) -C $(GAP_SDK_HOME)/docs clean

else
sdk:: all autotiler
all:: pulp-os gvsoc
clean:
	$(RM) $(TARGET_INSTALL_DIR)
	$(RM) $(BUILD_DIR)
endif

$(TARGET_INSTALL_DIR):
	$(MKDIR) -p $@

$(INSTALL_DIR): $(TARGET_INSTALL_DIR)
	$(MKDIR) -p $@/include

# Rules for installing docs
#------------------------------------------
docs:
	$(MAKE) -C $(GAP_SDK_HOME)/docs all

# Rules for installing tools
#------------------------------------------
INSTALL_BIN = $(INSTALL_DIR)/bin

install_bin:
	$(MKDIR) $(INSTALL_BIN)

install_others: install_bin
	$(CP) $(GAP_SDK_HOME)/tools/runner $(INSTALL_DIR)
	$(CP) $(GAP_SDK_HOME)/tools/bin/* $(INSTALL_DIR)/bin
	$(CP) $(GAP_SDK_HOME)/tools/ld $(INSTALL_DIR)
	$(CP) $(GAP_SDK_HOME)/tools/rules $(INSTALL_DIR)

install_pulp_tools: install_others
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools all

tools: install_others install_pulp_tools

nntool:
	$(MAKE) -C $(GAP_SDK_HOME)/tools/nntool all

pulp-os: $(TARGET_INSTALL_DIR) install_pulp_tools
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/pmsis_api -f tools/export.mk build
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pulp build
	$(MAKE) -C $(GAP_SDK_HOME)/rtos/pmsis/pmsis_bsp all
	$(MAKE) -C $(GAP_SDK_HOME)/libs/gap_lib all 

flasher: pulp-os
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools/gap_flasher install

gvsoc: pulp-os tools
	./gvsoc/gvsoc-build

autotiler:
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v2/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v2 all; fi
	if [ -e $(GAP_SDK_HOME)/tools/autotiler_v3/Makefile ]; then $(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler_v3 all; fi

openocd:
	cd tools/gap8-openocd && ./bootstrap
	cd tools/gap8-openocd && ./configure --prefix=$(INSTALL_DIR)
	cd tools/gap8-openocd && make install
	mkdir -p $(INSTALL_DIR)/share/openocd/scripts/tcl
	cd tools/gap8-openocd-tools && cp -r tcl/* $(INSTALL_DIR)/share/openocd/scripts/tcl
	cd tools/gap8-openocd-tools && mkdir -p $(INSTALL_DIR)/share/openocd/gap_bins && cp -r gap_bins/* $(INSTALL_DIR)/share/openocd/gap_bins

.PHONY: all install clean docs install_others install_pulp_tools tools pulp-os gvsoc flasher
