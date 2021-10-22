set(SOC_EU_PREFIX "pulp/soc_eu")

vp_model(NAME soc_eu_v1_impl
    PREFIX ${SOC_EU_PREFIX}
    SOURCES "soc_eu_v1_impl.cpp"
    )

vp_model(NAME soc_eu_v2_impl
    PREFIX ${SOC_EU_PREFIX}
    SOURCES "soc_eu_v2_impl.cpp"
    )

vp_model(NAME soc_eu_v3_impl
    PREFIX ${SOC_EU_PREFIX}
    SOURCES "soc_eu_v3_impl.cpp"
    )
