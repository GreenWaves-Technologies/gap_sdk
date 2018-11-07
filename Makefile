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

CD = cd
CP = cp -rf
RM = rm -rf
MKDIR = mkdir -p
MAKE = make

PULP_BRIDGE_PATH = $(GAP_SDK_HOME)/tools/pulp_tools/pulp-debug-bridge

install: pulp-os tools docs

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

$(INSTALL_BIN):
	$(MKDIR) $@

install_others: $(INSTALL_BIN)
	$(CP) $(GAP_SDK_HOME)/tools/runner $(INSTALL_DIR)
	$(CP) $(GAP_SDK_HOME)/tools/bin/* $(INSTALL_DIR)/bin
	$(CP) $(GAP_SDK_HOME)/tools/ld $(INSTALL_DIR)
	$(CP) $(GAP_SDK_HOME)/tools/rules $(INSTALL_DIR)

install_pulp_tools: install_others
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools all

tools: install_others install_pulp_tools

pulp-os: $(TARGET_INSTALL_DIR) install_pulp_tools
	$(MAKE) -C $(GAP_SDK_HOME)/pulp-os all

flasher: pulp-os
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools/gap_flasher install

autotiler:
	$(MAKE) -C $(GAP_SDK_HOME)/tools/autotiler all

version:
	@$(MBED_PATH)/tools/version/record_version.sh

all:: install flasher version

clean:
	$(RM) $(TARGET_INSTALL_DIR)
	$(RM) $(INSTALL_DIR)
	$(RM) $(BUILD_DIR)
	$(MAKE) -C $(GAP_SDK_HOME)/tools/pulp_tools clean
	$(MAKE) -C $(GAP_SDK_HOME)/docs clean
	$(RM) version.log

.PHONY: all install clean docs version install_others install_pulp_tools tools pulp-os flasher autotiler
