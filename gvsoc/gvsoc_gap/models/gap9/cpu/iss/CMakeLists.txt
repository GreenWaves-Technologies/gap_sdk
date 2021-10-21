set(GAP9_ISS_WRAPPER "gap9_iss_wrapper.cpp")

generate_isa(NAME "iss_gap9_fc"
    THINGY "--core=Gap9_fc_core@gap9.cpu.iss.gap9_cores"
    PREFIX "gap9/cpu/iss"
    SOURCES ${GAP9_ISS_WRAPPER}
    )
vp_model_compile_definitions(
    NAME iss_gap9_fc
    DEFINITIONS
    -DSECURE=1
    -DEXTERNAL_ISS_WRAPPER=1
    -DPIPELINE_STAGES=2
    -DISS_SINGLE_REGFILE
    -DPCER_VERSION_2
    -DPRIV_1_10
    )

generate_isa(NAME iss_gap9_cluster
    THINGY "--core=Gap9_cluster_core@gap9.cpu.iss.gap9_cores"
    PREFIX "gap9/cpu/iss"
    SOURCES ${GAP9_ISS_WRAPPER}
    )
vp_model_compile_definitions(
    NAME iss_gap9_cluster
    DEFINITIONS
    -DEXTERNAL_ISS_WRAPPER=1
    -DPIPELINE_STAGES=2
    -DISS_SINGLE_REGFILE
    -DPCER_VERSION_2
    -DPRIV_1_10
    )
