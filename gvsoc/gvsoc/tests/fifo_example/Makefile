# Directory used for temporary files
ROOT_VP_BUILD_DIR ?= $(CURDIR)/build

# Directory where models are installed
VP_INSTALL_PATH = $(CURDIR)/build/lib

# Implementations C++ components which must be compiled.
# The name is is the one which can be used from inside a python component
# to include an implementation.
IMPLEMENTATIONS += master_impl slave_impl

# Python components to be installed.
# The name here is both the name of the python file without the .py extension
# and the name used from Python to instantiate this component.
COMPONENTS += master slave top

# For each implementation file, the associated C++ files must be specified.
master_impl_SRCS = master_impl.cpp
slave_impl_SRCS = slave_impl.cpp


build: vp_build

clean: vp_clean

run:
	pulp-run --platform=vp --dir=$(CURDIR)/work --config-file=$(CURDIR)/config.json
	

include $(PULP_SDK_HOME)/install/rules/vp_models.mk


.PHONY: clean build run
