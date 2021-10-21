set(HWCE_PREFIX "pulp/hwce")

vp_model(NAME hwce_v4_impl
    PREFIX ${HWCE_PREFIX}
    SOURCES "hwce_v4_impl.cpp"
    )

vp_model(NAME hwce_v5_impl
    PREFIX ${HWCE_PREFIX}
    SOURCES "hwce_v5_impl.cpp"
    )
