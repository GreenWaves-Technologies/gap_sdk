ROOT_VP_BUILD_DIR ?= $(CURDIR)/build

IMPLEMENTATIONS += master_impl slave_impl

COMPONENTS += master slave top

master_impl_SRCS = master_impl.cpp
slave_impl_SRCS = slave_impl.cpp


build: vp_build

clean: vp_clean

run:
	pulp-run --platform=vp --dir=$(CURDIR)/work --config-file=$(CURDIR)/config.json
	

include $(PULP_SDK_HOME)/install/rules/vp_models.mk


.PHONY: clean build run
