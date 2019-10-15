VP_BUILD_DIR = $(ROOT_VP_BUILD_DIR)/models

VP_INSTALL_PATH ?= $(INSTALL_DIR)/lib
VP_PY_INSTALL_PATH ?= $(INSTALL_DIR)/python

VP_MAKEFILE_LIST = $(addsuffix /Makefile,$(VP_DIRS))

CPP=g++
CC=gcc

VP_COMP_PYBIND_FLAGS := $(shell python3-config --includes)

VP_COMP_CFLAGS=-MMD -MP -O2 -g -fpic -D__GVSOC__=1 -I$(INSTALL_DIR)/include $(VP_COMP_PYBIND_FLAGS)

ifdef ARCHI_DIR
VP_COMP_CFLAGS += -I$(ARCHI_DIR)
endif

VP_COMP_CPPFLAGS=-std=c++11
VP_COMP_LDFLAGS=-O2 -g -shared -L$(INSTALL_DIR)/lib
VP_COMP_STD_LDFLAGS=-lpulpvp
VP_COMP_DBG_LDFLAGS=-lpulpvp-debug

VP_COMP_CFLAGS += -Werror -Wfatal-errors
VP_COMP_LDFLAGS += -Werror -Wfatal-errors

ifdef VP_USE_SYSTEMC
VP_COMP_CFLAGS += -D__VP_USE_SYSTEMC -I$(SYSTEMC_HOME)/include
ifdef VP_USE_SYSTEMC_DRAMSYS
DRAMSYS = $(PWD)/platform/gvsoc/models/memory/dram.sys
VP_COMP_CFLAGS += -D__VP_USE_SYSTEMC_DRAMSYS
VP_COMP_CFLAGS += -D__DRAMSYS_PATH=\"$(DRAMSYS)\"
VP_COMP_CFLAGS += -I$(DRAMSYS)/DRAMSys/library/src/simulation
VP_COMP_CFLAGS += -I$(DRAMSYS)/DRAMSys/library/src/common/third_party/DRAMPower/src
VP_COMP_CFLAGS += -I$(DRAMSYS)/DRAMSys/library/src/common/third_party/DRAMPower/src/libdrampower
VP_COMP_LDFLAGS += -L$(DRAMSYS)/build/library -lDRAMSys
VP_COMP_LDFLAGS += -L$(DRAMSYS)/DRAMSys/library/src/common/third_party/DRAMPower/src -ldrampower
VP_COMP_LDFLAGS += -lsqlite3
endif # VP_USE_SYSTEMC_DRAMSYS
endif # VP_USE_SYSTEMC

export PYTHONPATH := $(VP_INSTALL_PATH):$(INSTALL_DIR)/lib:$(PYTHONPATH)







VP_COMP_EXT := .so
#$(shell python3-config --extension-suffix)

include $(VP_MAKEFILE_LIST)


define declare_implementation

$(eval $(1)_OBJS = $(patsubst %.cc, $(VP_BUILD_DIR)/$(1)/%.o, $(patsubst %.c, $(VP_BUILD_DIR)/$(1)/%.o, $(patsubst %.cpp, $(VP_BUILD_DIR)/$(1)/%.o, $($(1)_SRCS)))))

-include $($(1)_OBJS:.o=.d)

$(VP_BUILD_DIR)/$(1)/%.o: %.cpp $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CPP) -c $$< -o $$@ $($(1)_CFLAGS) $($(1)_CPPFLAGS) $(VP_COMP_CFLAGS) $(VP_COMP_CPPFLAGS)

$(VP_BUILD_DIR)/$(1)/%.o: %.cc $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CPP) -c $$< -o $$@ $($(1)_CFLAGS) $($(1)_CPPFLAGS) $(VP_COMP_CFLAGS) $(VP_COMP_CPPFLAGS)

$(VP_BUILD_DIR)/$(1)/%.o: %.c $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CC) -c $$< -o $$@ $($(1)_CFLAGS) $(VP_COMP_CFLAGS)


$(VP_BUILD_DIR)/$(1)$(VP_COMP_EXT): $($(1)_OBJS) $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CPP) $($(1)_OBJS) -o $$@ $($(1)_CFLAGS) $(VP_COMP_CFLAGS) $($(1)_LDFLAGS) $(VP_COMP_LDFLAGS) $(VP_COMP_STD_LDFLAGS)

$(VP_PY_INSTALL_PATH)/$(1)$(VP_COMP_EXT): $(VP_BUILD_DIR)/$(1)$(VP_COMP_EXT)
	install -D $$^ $$@

VP_INSTALL_TARGETS += $(VP_PY_INSTALL_PATH)/$(1)$(VP_COMP_EXT)

endef



define declare_debug_implementation

$(eval $(1)_DBG_OBJS = $(patsubst %.cc, $(VP_BUILD_DIR)/$(1)/debug/%.o, $(patsubst %.c, $(VP_BUILD_DIR)/$(1)/debug/%.o, $(patsubst %.cpp, $(VP_BUILD_DIR)/$(1)/debug/%.o, $($(1)_SRCS)))))

-include $($(1)_DBG_OBJS:.o=.d)

$(VP_BUILD_DIR)/$(1)/debug/%.o: %.cpp $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CPP) -c $$< -o $$@ $($(1)_CFLAGS) $(VP_COMP_CFLAGS) $($(1)_CPPFLAGS) $(VP_COMP_CPPFLAGS) -DVP_TRACE_ACTIVE=1

$(VP_BUILD_DIR)/$(1)/debug/%.o: %.cc $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CPP) -c $$< -o $$@ $($(1)_CFLAGS) $(VP_COMP_CFLAGS) $($(1)_CPPFLAGS) $(VP_COMP_CPPFLAGS) -DVP_TRACE_ACTIVE=1

$(VP_BUILD_DIR)/$(1)/debug/%.o: %.c $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CC) -c $$< -o $$@ $($(1)_CFLAGS) $(VP_COMP_CFLAGS) -DVP_TRACE_ACTIVE=1


$(VP_BUILD_DIR)/debug/$(1)$(VP_COMP_EXT): $($(1)_DBG_OBJS) $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CPP) $($(1)_DBG_OBJS) -o $$@ $($(1)_CFLAGS) $(VP_COMP_CFLAGS) $($(1)_LDFLAGS) $(VP_COMP_LDFLAGS) $(VP_COMP_DBG_LDFLAGS)

$(VP_PY_INSTALL_PATH)/debug/$(1)$(VP_COMP_EXT): $(VP_BUILD_DIR)/debug/$(1)$(VP_COMP_EXT)
	install -D $$^ $$@

VP_INSTALL_TARGETS += $(VP_PY_INSTALL_PATH)/debug/$(1)$(VP_COMP_EXT)

endef



define declare_component

$(VP_PY_INSTALL_PATH)/$(1).py: $(1).py
	install -D $$^ $$@

VP_INSTALL_TARGETS += $(VP_PY_INSTALL_PATH)/$(1).py

endef


define declareInstallFile

$(INSTALL_DIR)/include/$(1): $(1)
	install -D $(1) $$@

VP_INSTALL_HEADERS += $(INSTALL_DIR)/include/$(1)

endef


$(foreach implementation, $(IMPLEMENTATIONS), $(eval $(call declare_implementation,$(implementation))))

$(foreach implementation, $(IMPLEMENTATIONS), $(eval $(call declare_debug_implementation,$(implementation))))

$(foreach component, $(COMPONENTS), $(eval $(call declare_component,$(component))))

$(foreach file, $(VP_HEADERS), $(eval $(call declareInstallFile,$(file))))



vp_build: $(VP_INSTALL_HEADERS) $(VP_INSTALL_TARGETS)
	find $(VP_PY_INSTALL_PATH) -type d -exec touch {}/__init__.py \;

vp_clean:
	rm -rf $(VP_BUILD_DIR)
