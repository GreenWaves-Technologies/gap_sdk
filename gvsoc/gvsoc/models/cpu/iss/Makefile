COMMON_SRCS = cpu/iss/vp/src/iss_wrapper.cpp cpu/iss/src/iss.cpp cpu/iss/src/insn_cache.cpp cpu/iss/src/csr.cpp cpu/iss/src/decoder.cpp cpu/iss/src/trace.cpp cpu/iss/flexfloat/flexfloat.c

COMMON_CFLAGS = -DRISCV=1 -DRISCY -I$(CURDIR)/cpu/iss/include -I$(CURDIR)/cpu/iss/vp/include -I$(CURDIR)/cpu/iss/flexfloat -march=native -fno-strict-aliasing

ifdef USE_TRDB
COMMON_CFLAGS += -DUSE_TRDB=1
COMMON_LDFLAGS = -ltrdb -lbfd -lopcodes -liberty -lz
endif


define declare_iss_isa_build

$(VP_BUILD_DIR)/cpu/iss/iss_wrapper/$(1)_decoder_gen.hpp: cpu/iss/isa_gen/isa_riscv_gen.py cpu/iss/isa_gen/isa_gen.py
	./cpu/iss/isa_gen/isa_riscv_gen.py --source-file=$(VP_BUILD_DIR)/cpu/iss/iss_wrapper/$(1)_decoder_gen.cpp --header-file=$(VP_BUILD_DIR)/cpu/iss/iss_wrapper/$(1)_decoder_gen.hpp $(2)

$(VP_BUILD_DIR)/cpu/iss/iss_wrapper/$(1)_decoder_gen.cpp: cpu/iss/isa_gen/isa_riscv_gen.py cpu/iss/isa_gen/isa_gen.py
	./cpu/iss/isa_gen/isa_riscv_gen.py --source-file=$(VP_BUILD_DIR)/cpu/iss/iss_wrapper/$(1)_decoder_gen.cpp --header-file=$(VP_BUILD_DIR)/cpu/iss/iss_wrapper/$(1)_decoder_gen.hpp $(2)

endef

$(eval $(call declare_iss_isa_build,riscy))
$(eval $(call declare_iss_isa_build,zeroriscy,--implem=zeroriscy))


cpu/iss/iss_zeroriscy_CFLAGS += -DPIPELINE_STAGES=1 -DISS_SINGLE_REGFILE
cpu/iss/iss_zeroriscy_SRCS += $(VP_BUILD_DIR)/cpu/iss/iss_wrapper/zeroriscy_decoder_gen.cpp
cpu/iss/iss_zeroriscy_SRCS += $(COMMON_SRCS)
cpu/iss/iss_zeroriscy_CFLAGS += $(COMMON_CFLAGS)
cpu/iss/iss_zeroriscy_LDFLAGS += $(COMMON_LDFLAGS)

cpu/iss/iss_riscy_single_regfile_CFLAGS += -DPIPELINE_STAGES=2 -DISS_SINGLE_REGFILE
cpu/iss/iss_riscy_single_regfile_SRCS += $(VP_BUILD_DIR)/cpu/iss/iss_wrapper/riscy_decoder_gen.cpp
cpu/iss/iss_riscy_single_regfile_SRCS += $(COMMON_SRCS)
cpu/iss/iss_riscy_single_regfile_CFLAGS += $(COMMON_CFLAGS)
cpu/iss/iss_riscy_single_regfile_LDFLAGS += $(COMMON_LDFLAGS)

cpu/iss/iss_riscy_v2_5_single_regfile_CFLAGS += -DPIPELINE_STAGES=2 -DISS_SINGLE_REGFILE -DPCER_VERSION_2 -DPRIV_1_10
cpu/iss/iss_riscy_v2_5_single_regfile_SRCS += $(VP_BUILD_DIR)/cpu/iss/iss_wrapper/riscy_decoder_gen.cpp
cpu/iss/iss_riscy_v2_5_single_regfile_SRCS += $(COMMON_SRCS)
cpu/iss/iss_riscy_v2_5_single_regfile_CFLAGS += $(COMMON_CFLAGS)
cpu/iss/iss_riscy_v2_5_single_regfile_LDFLAGS += $(COMMON_LDFLAGS)

cpu/iss/iss_riscy_v2_5_single_regfile_secure_CFLAGS += -DPIPELINE_STAGES=2 -DISS_SINGLE_REGFILE -DPCER_VERSION_2 -DPRIV_1_10 -DSECURE=1
cpu/iss/iss_riscy_v2_5_single_regfile_secure_SRCS += $(VP_BUILD_DIR)/cpu/iss/iss_wrapper/riscy_decoder_gen.cpp
cpu/iss/iss_riscy_v2_5_single_regfile_secure_SRCS += $(COMMON_SRCS)
cpu/iss/iss_riscy_v2_5_single_regfile_secure_CFLAGS += $(COMMON_CFLAGS)
cpu/iss/iss_riscy_v2_5_single_regfile_secure_LDFLAGS += $(COMMON_LDFLAGS)

cpu/iss/iss_riscy_v2_5_CFLAGS += -DPIPELINE_STAGES=2 -DPCER_VERSION_2 -DPRIV_1_10
cpu/iss/iss_riscy_v2_5_SRCS += $(VP_BUILD_DIR)/cpu/iss/iss_wrapper/riscy_decoder_gen.cpp
cpu/iss/iss_riscy_v2_5_SRCS += $(COMMON_SRCS)
cpu/iss/iss_riscy_v2_5_CFLAGS += $(COMMON_CFLAGS)
cpu/iss/iss_riscy_v2_5_LDFLAGS += $(COMMON_LDFLAGS)

cpu/iss/iss_riscy_CFLAGS += -DPIPELINE_STAGES=2
cpu/iss/iss_riscy_SRCS += $(VP_BUILD_DIR)/cpu/iss/iss_wrapper/riscy_decoder_gen.cpp
cpu/iss/iss_riscy_SRCS += $(COMMON_SRCS)
cpu/iss/iss_riscy_CFLAGS += $(COMMON_CFLAGS)
cpu/iss/iss_riscy_LDFLAGS += $(COMMON_LDFLAGS)


define declare_iss_check_class

ifneq (,$(findstring iss_zeroriscy,$(1)))

IMPLEMENTATIONS += cpu/iss/iss_zeroriscy

else
ifneq (,$(findstring iss_riscy_single_regfile,$(1)))

IMPLEMENTATIONS += cpu/iss/iss_riscy_single_regfile

else
ifneq (,$(findstring iss_riscy_v2_5_single_regfile_secure,$(1)))

IMPLEMENTATIONS += cpu/iss/iss_riscy_v2_5_single_regfile_secure

else
ifneq (,$(findstring iss_riscy_v2_5_single_regfile,$(1)))

IMPLEMENTATIONS += cpu/iss/iss_riscy_v2_5_single_regfile

else
ifneq (,$(findstring iss_riscy_v2_5,$(1)))

IMPLEMENTATIONS += cpu/iss/iss_riscy_v2_5

else
ifneq (,$(findstring iss_riscy,$(1)))

IMPLEMENTATIONS += cpu/iss/iss_riscy

endif
endif
endif
endif
endif
endif

endef

ifneq '$(fc/iss_class)' ''
$(info FC $(fc/iss_class) )

$(eval $(call declare_iss_check_class,$(fc/iss_class)))
endif

ifneq '$(pe/iss_class)' ''
ifneq '$(pe/iss_class)' '$(fc/iss_class)'
$(info PE $(pe/iss_class) )
$(eval $(call declare_iss_check_class,$(pe/iss_class)))
endif
endif


$(info IMP $(IMPLEMENTATIONS))