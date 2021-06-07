include $(GVSOC_ISS_PATH)/iss.mk



ifeq '$(TARGET_CHIP)' 'GAP9_V2'


# GAP9 cluster core

$(eval $(call declare_iss_isa_build,gap9/cpu/iss/iss_wrapper/gap9_cluster_core,--core=Gap9_cluster_core@gap9.cpu.iss.gap9_cores,gap9/cpu/iss/gap9_cores.py))


IMPLEMENTATIONS += gap9/cpu/iss/iss_gap9_cluster

gap9/cpu/iss/iss_gap9_cluster_CFLAGS += -DEXTERNAL_ISS_WRAPPER=1 -DPIPELINE_STAGES=2 -DISS_SINGLE_REGFILE -DPCER_VERSION_2 -DPRIV_1_10
gap9/cpu/iss/iss_gap9_cluster_SRCS += $(VP_BUILD_DIR)/gap9/cpu/iss/iss_wrapper/gap9_cluster_core_decoder_gen.cpp gap9/cpu/iss/gap9_iss_wrapper.cpp
gap9/cpu/iss/iss_gap9_cluster_SRCS += $(COMMON_SRCS)
gap9/cpu/iss/iss_gap9_cluster_CFLAGS += $(COMMON_CFLAGS)
gap9/cpu/iss/iss_gap9_cluster_LDFLAGS += $(COMMON_LDFLAGS)




# GAP9 FC core

$(eval $(call declare_iss_isa_build,gap9/cpu/iss/iss_wrapper/gap9_fc_core,--core=Gap9_fc_core@gap9.cpu.iss.gap9_cores,gap9/cpu/iss/gap9_cores.py))

IMPLEMENTATIONS += gap9/cpu/iss/iss_gap9_fc

gap9/cpu/iss/iss_gap9_fc_CFLAGS += -DSECURE=1 -DEXTERNAL_ISS_WRAPPER=1 -DPIPELINE_STAGES=2 -DISS_SINGLE_REGFILE -DPCER_VERSION_2 -DPRIV_1_10
gap9/cpu/iss/iss_gap9_fc_SRCS += $(VP_BUILD_DIR)/gap9/cpu/iss/iss_wrapper/gap9_fc_core_decoder_gen.cpp gap9/cpu/iss/gap9_iss_wrapper.cpp
gap9/cpu/iss/iss_gap9_fc_SRCS += $(COMMON_SRCS)
gap9/cpu/iss/iss_gap9_fc_CFLAGS += $(COMMON_CFLAGS)
gap9/cpu/iss/iss_gap9_fc_LDFLAGS += $(COMMON_LDFLAGS)


endif