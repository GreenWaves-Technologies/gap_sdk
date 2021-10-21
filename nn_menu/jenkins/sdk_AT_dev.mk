SHELL=/bin/bash

ifndef GAP_RISCV_GCC_TOOLCHAIN
export GAP_RISCV_GCC_TOOLCHAIN=$(GAP_RISCV_GCC_TOOLCHAIN_BASE)/1.4
endif

PLPTEST_PLATFORM ?= gvsoc
PMSIS_OS ?= pulpos
JENKINS_CONFIG ?= gap8_v2
PLPTEST_OPT ?= --max-timeout=360

export PMSIS_OS
export PLPTEST_PLATFORM

TESTSET ?= --testset=testset.cfg

gap_sdk_checkout:
	if [ ! -e gap_sdk ]; then git clone git@gitlab.greenwaves-tech.com:gwt_sdk_developer/gap_sdk.git; fi
	cd gap_sdk && git checkout integration && git submodule update --init --recursive

gap_sdk_build:
	cd gap_sdk && . configs/$(JENKINS_CONFIG).sh && make sdk

bridge:
	if [ ! -e pulp-debug-bridge ]; then git clone git@github.com:pulp-platform/pulp-debug-bridge.git; fi
	. gap_sdk/configs/$(JENKINS_CONFIG).sh && cd pulp-debug-bridge && make INSTALL_DIR=$$GAP_SDK_HOME/install/workstation TARGET_INSTALL_DIR=$$GAP_SDK_HOME/install BUILD_DIR=$$GAP_SDK_HOME/build/bridge build install

nn_menu:
	git submodule update --init --recursive && echo https://greenwaves-technologies.com/shutterlesslibrary/ > main_courses/occupancy_management/.shutterless_url

autotiler:
	git clone git@gitlab.greenwaves-tech.com:gwt-property-sw/AutotilerV2.git
	source gap_sdk/configs/$(JENKINS_CONFIG).sh && cd AutotilerV2 && git checkout test_Eric_New_merge && source sourceme.sh <<< y && make clean install


sdk: gap_sdk_checkout autotiler nn_menu gap_sdk_build bridge

test:
	. gap_sdk/configs/$(JENKINS_CONFIG).sh && plptest $(TESTSET) $(PLPTEST_OPT)

