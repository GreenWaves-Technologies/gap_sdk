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

export INSTALL_DIR
export BUILD_DIR

ifndef CMAKE
HAS_CMAKE3 = $(shell which cmake3)
ifeq '$(HAS_CMAKE3)' ''
CMAKE = $(shell which cmake-3.7.1)
ifeq '$(CMAKE)' ''
CMAKE = cmake
endif
else
CMAKE = cmake3
endif
endif

export PATH := $(CURDIR)/bin:$(PATH)

$(info #### Building in $(BUILD_DIR))
$(info #### Release type is $(RELEASE_TYPE))
$(info #### Installing to $(INSTALL_DIR))
# $(info #### Installing target files to $(TARGET_INSTALL_DIR))

MAKEFILE_DIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

all_scons:
	scons -Q

all: install

install: $(BUILD_DIR)/Makefile
	( cd $(BUILD_DIR) ; make install $(DBG_CMAKE) VERBOSE=$(VERBOSE) )

sdk_install: install
	@mkdir -p $(TARGET_INSTALL_DIR)/rules
	touch $(TARGET_INSTALL_DIR)/rules/tools.mk

clean:
	rm -f .sconsign.dblite
	rm -rf $(BUILD_DIR)

gen.all:
	./bin/plpconf_new --input=chips/pulp/pulp.json --output=configs/systems/pulp.json
	./bin/plpconf_new --input=chips/pulpissimo_v2/pulpissimo_v2.json --output=configs/systems/pulpissimo_v2.json
	./bin/plpconf_new --input=chips/pulpissimo/pulpissimo.json --output=configs/systems/pulpissimo.json
	./bin/plpconf_new --input=chips/pulpissimo-zeroriscy/pulpissimo-zeroriscy.json --output=configs/systems/pulpissimo-zeroriscy.json
	./bin/plpconf_new --input=chips/pulpissimo-microriscy/pulpissimo-microriscy.json --output=configs/systems/pulpissimo-microriscy.json
	./bin/plpconf_new --input=chips/pulpissimo-riscy/pulpissimo-riscy.json --output=configs/systems/pulpissimo-riscy.json
	./bin/plpconf_new --input=chips/arnold/arnold.json --output=configs/systems/arnold.json
	./bin/plpconf_new --input=chips/gap/gap.json --output=configs/systems/gap.json
	./bin/plpconf_new --input=chips/gap_rev1/gap_rev1.json --output=configs/systems/gap_rev1.json
	./bin/plpconf_new --input=chips/wolfe/wolfe.json --output=configs/systems/wolfe.json
	./bin/plpconf_new --input=chips/vega/vega.json --output=configs/systems/vega.json
	./bin/plpconf_new --input=chips/vivosoc3/vivosoc3.json --output=configs/systems/vivosoc3.json
	./bin/plpconf_new --input=chips/vivosoc3_5/vivosoc3_5.json --output=configs/systems/vivosoc3_5.json
	./bin/plpconf_new --input=chips/multino/multino.json --output=configs/systems/multino.json
	./bin/plpconf_new --input=chips/bigpulp-standalone/bigpulp.json --output=configs/systems/bigpulp-standalone.json
	./bin/plpconf_new --input=chips/bigpulp/bigpulp.json --output=configs/systems/bigpulp.json
	./bin/plpconf_new --input=chips/bigpulp-juno/bigpulp-juno.json --output=configs/systems/bigpulp-juno.json
	./bin/plpconf_new --input=chips/bigpulp-z-7045/bigpulp-z-7045.json --output=configs/systems/bigpulp-z-7045.json
	./bin/plpconf_new --input=chips/bigpulp-zu9eg/bigpulp-zu9eg.json --output=configs/systems/bigpulp-zu9eg.json
	./bin/plpconf_new --input=chips/hero-z-7045/hero-z-7045.json --output=configs/systems/hero-z-7045.json
	./bin/plpconf_new --input=chips/oprecompkw_sa/oprecompkw_sa.json --output=configs/systems/oprecompkw_sa.json
	./bin/plpconf_new --input=chips/oprecompkw/oprecompkw.json --output=configs/systems/oprecompkw.json
	./bin/plpconf_new --input=chips/oprecompkw_sfloat/oprecompkw_sfloat.json --output=configs/systems/oprecompkw_sfloat.json
	./bin/plpconf_new --input=chips/oprecompkw_sfloat_sa/oprecompkw_sfloat_sa.json --output=configs/systems/oprecompkw_sfloat_sa.json
	./bin/plpconf_new --input=chips/oprecompkw_sfloat_sa_dual_regfile/oprecompkw_sfloat_sa_dual_regfile.json --output=configs/systems/oprecompkw_sfloat_sa_dual_regfile.json
	./bin/plpconf_new --input=chips/usoc_v1/usoc_v1.json --output=configs/systems/usoc_v1.json


gen: gen.all

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/Makefile: $(BUILD_DIR)
	( cd $(BUILD_DIR) ; \
	  $(CMAKE) -DCMAKE_BUILD_TYPE=$(RELEASE_TYPE) -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR) \
		$(EXTRA_CMAKE_ARGS) $(MAKEFILE_DIR) )
