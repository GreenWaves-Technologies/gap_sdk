IMPLEMENTATIONS += memory/memory_impl
memory/memory_impl_SRCS = memory/memory_impl.cpp

IMPLEMENTATIONS += memory/ddr_impl
ifdef VP_USE_SYSTEMC

memory/ddr_impl_SRCS = memory/ddr_sc_impl.cpp
ifdef VP_USE_SYSTEMC_GEM5
VP_USE_SYSTEMC_GEM5_HOME ?= $(HOME)/gem5_tnt/gem5
GEM5_HOME = $(VP_USE_SYSTEMC_GEM5_HOME)
VP_USE_SYSTEMC_GEM5_LIB ?= $(GEM5_HOME)/build/ARM/libgem5_opt.so
GEM5_LIB = $(VP_USE_SYSTEMC_GEM5_LIB)
$(info Gem5 home is $(GEM5_HOME))
$(info Gem5 library is $(GEM5_LIB))
ifeq (,$(wildcard $(GEM5_LIB)))
$(info Required library $(GEM5_LIB) not found)
$(info Please check $(shell find . -name README-gem5.md -exec readlink -f {} \;))
$(info You may want to execute $(shell find . -name gem5.tnt.sh -exec readlink -f {} \;))
$(error Required library $(GEM5_LIB) not found)
else
$(info Library $(GEM5_LIB) found)
endif
VP_COMP_CFLAGS += -D__VP_USE_SYSTEMC_GEM5
VP_COMP_CFLAGS += -D__GEM5_PATH=\"$(GEM5_HOME)\"
VP_COMP_CFLAGS += -I$(GEM5_HOME)/build/ARM
VP_COMP_CFLAGS += -I$(GEM5_HOME)/util/tlm/examples/slave_port
VP_COMP_CFLAGS += -I$(GEM5_HOME)/util/tlm/examples/common
VP_COMP_CFLAGS += -I$(GEM5_HOME)/util/tlm/src
VP_COMP_CFLAGS += -I$(GEM5_HOME)/util/systemc
VP_COMP_LDFLAGS += -L$(GEM5_HOME)/build/ARM -lgem5_opt
extra_srcs += $(filter-out $(GEM5_HOME)/util/systemc/main.cc, $(wildcard $(GEM5_HOME)/util/systemc/*.cc))
extra_srcs += $(wildcard $(GEM5_HOME)/util/tlm/examples/common/*.cc)
extra_srcs += $(wildcard $(GEM5_HOME)/util/tlm/src/*.cc)
memory/ddr_impl_SRCS += $(extra_srcs)
endif # VP_USE_SYSTEMC_GEM5
else
memory/ddr_impl_SRCS = memory/ddr_impl.cpp
endif # VP_USE_SYSTEMC
