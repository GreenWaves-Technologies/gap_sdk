#
# Copyright (C) 2018 ETH Zurich and University of Bologna and GreenWaves Technologies SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Authors: Martin Croome, GreenWaves Technologies

INSTALL_DIR ?= $(CURDIR)/install
# TARGET_INSTALL_DIR ?= $(CURDIR)/install
BUILD_DIR   ?= $(CURDIR)/build
RELEASE_TYPE ?= Debug
# propagate verbose for debugging
VERBOSE ?= 0

$(info #### Building in $(BUILD_DIR))
$(info #### Release type is $(RELEASE_TYPE))
$(info #### Installing to $(INSTALL_DIR))
# $(info #### Installing target files to $(TARGET_INSTALL_DIR))

ifeq ($(DONT_USE_SDL),"")
  EXTRA_CMAKE_ARGS+=-DWithFrameBuffer
endif

MAKEFILE_DIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

all: $(BUILD_DIR)/Makefile
	( cd $(BUILD_DIR) ; make all $(DBG_CMAKE) VERBOSE=$(VERBOSE) )
	make -C pulp-rt-bridge all

install: $(BUILD_DIR)/Makefile
	( cd $(BUILD_DIR) ; make install $(DBG_CMAKE) VERBOSE=$(VERBOSE) )

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/Makefile: $(BUILD_DIR)
	( cd $(BUILD_DIR) ; \
	  cmake -DCMAKE_BUILD_TYPE=$(RELEASE_TYPE) -DWithFlasher=ON -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR) -DTARGET_INSTALL_DIR=$(TARGET_INSTALL_DIR)\
		$(EXTRA_CMAKE_ARGS) $(MAKEFILE_DIR) )
