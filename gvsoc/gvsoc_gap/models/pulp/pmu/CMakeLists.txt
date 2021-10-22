set(PMU_PREFIX "pulp/pmu")

vp_model(NAME pmu_v1_impl
    PREFIX ${PMU_PREFIX}
    SOURCES "pmu_v1_impl.cpp"
    )

vp_model(NAME pmu_v2_impl
    PREFIX ${PMU_PREFIX}
    SOURCES "pmu_v2_impl.cpp"
    )

vp_model(NAME pmu_v3_impl
    PREFIX ${PMU_PREFIX}
    SOURCES "pmu_v3_impl.cpp"
    )

vp_model(NAME pmu_v4_impl
    PREFIX ${PMU_PREFIX}
    SOURCES "pmu_v4_impl.cpp"
    )
