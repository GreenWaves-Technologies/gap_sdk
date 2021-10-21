SHELL=/bin/bash

ifndef GAP_RISCV_GCC_TOOLCHAIN
export GAP_RISCV_GCC_TOOLCHAIN=$(GAP_RISCV_GCC_TOOLCHAIN_BASE)/1.4
endif

PLPTEST_PLATFORM ?= gvsoc
PMSIS_OS ?= pulpos
JENKINS_CONFIG ?= gap8_v2
PLPTEST_OPT ?= --max-timeout=300

export PMSIS_OS
export PLPTEST_PLATFORM

TESTSET ?= --testset=testset.cfg

gap_sdk_checkout:
	if [ ! -e gap_sdk ]; then git clone git@github.com:GreenWaves-Technologies/gap_sdk.git; fi
	cd gap_sdk && git submodule update --init --recursive

gap_sdk_build:
	cd gap_sdk && . configs/$(JENKINS_CONFIG).sh && echo "https://greenwaves-technologies.com/autotiler/" > .tiler_url && make sdk

bridge:
	if [ ! -e pulp-debug-bridge ]; then git clone git@github.com:pulp-platform/pulp-debug-bridge.git; fi
	. gap_sdk/configs/$(JENKINS_CONFIG).sh && cd pulp-debug-bridge && make INSTALL_DIR=$$GAP_SDK_HOME/install/workstation TARGET_INSTALL_DIR=$$GAP_SDK_HOME/install BUILD_DIR=$$GAP_SDK_HOME/build/bridge build install

nn_menu:
	git submodule update --init --recursive && git submodule foreach git checkout master && git submodule foreach git pull && echo https://greenwaves-technologies.com/shutterlesslibrary/ > main_courses/occupancy_management/.shutterless_url

plp_test:
	cd gap_sdk && . configs/$(JENKINS_CONFIG).sh && git clone git@github.com:pulp-platform/plptest.git && cd plptest && make build

sdk: gap_sdk_checkout nn_menu gap_sdk_build bridge plp_test

test:
	. gap_sdk/configs/$(JENKINS_CONFIG).sh && plptest $(TESTSET) $(PLPTEST_OPT)

